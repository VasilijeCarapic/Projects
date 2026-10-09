/**
 ******************************************************************************
 * @file    ina234.c
 *
 * @brief   INA234 current, voltage and power monitor driver implementation.
 *
 *          Register map, calibration math and public API are taken as-is
 *          from Haris Turkmanovic's original CLP-master driver. Only the
 *          I2C transport (prvINA234_ReadReg/WriteReg) was rewritten to use
 *          the STM32 HAL I2C3 peripheral (hi2c3) directly, since this
 *          project does not use his DRV_I2C abstraction layer.
 *
 * @authors Haris Turkmanovic (original), Veljko Sokolov (HAL I2C1 port)
 * @date    September 2026
 ******************************************************************************
 */
#include <stdint.h>

#include <Sensors/ina234.h>
#include <main.h>

extern I2C_HandleTypeDef hi2c3;

/*
 * INA234A with A0 connected to GND.
 *
 * INA234A: A0=GND 0x40, VS 0x41, SDA 0x42, SCL 0x43
 * INA234B: A0=GND 0x48, VS 0x49, SDA 0x4A, SCL 0x4B
 *
 * TODO: confirm this matches your actual sensor/breakout - use the
 * existing I2C_Scan() (main.c) once to see which 7-bit address it
 * actually responds on, and change it here if it's not 0x40.
 */
#define INA234_DEV_ADDR                 0x40U

#define INA234_REG_CONFIG               0x00U
#define INA234_REG_SHUNT_VOLTAGE        0x01U
#define INA234_REG_BUS_VOLTAGE          0x02U
#define INA234_REG_POWER                0x03U
#define INA234_REG_CURRENT              0x04U
#define INA234_REG_CALIBRATION          0x05U
#define INA234_REG_MASK_ENABLE          0x06U
#define INA234_REG_ALERT_LIMIT          0x07U
#define INA234_REG_MANUFACTURER_ID      0x3EU
#define INA234_REG_DEVICE_ID            0x3FU

#define INA234_MANUFACTURER_ID          0x5449U
#define INA234_DEVICE_ID                0xA480U

#define INA234_CONFIG_RST_MASK          0x8000U
#define INA234_CONFIG_ADCRANGE_MASK     0x1000U
#define INA234_CONFIG_AVG_MASK          0x0E00U
#define INA234_CONFIG_VBUSCT_MASK       0x01C0U
#define INA234_CONFIG_VSHCT_MASK        0x0038U
#define INA234_CONFIG_MODE_MASK         0x0007U

#define INA234_SHUNT_LSB_RANGE0         0.000040f   /* 40 uV/LSB */
#define INA234_SHUNT_LSB_RANGE1         0.000010f   /* 10 uV/LSB */
#define INA234_BUS_VOLTAGE_LSB          0.0256f     /* 25.6 mV/LSB */

#define INA234_CALIBRATION_CONSTANT     0.08192f

static float prvINA234_CurrentLSB = 0.0f;
static ina234_adc_range_t prvINA234_ADCRange = INA234_ADC_RANGE_81_92_MV;

/**
 * @brief   Reads a 16-bit big-endian register over I2C3 (hi2c3).
 */
static ina234_status_t prvINA234_ReadReg(uint8_t reg, uint16_t* data, uint32_t timeout)
{
    uint8_t dataRx[2];

    if (data == 0)
    {
        return INA234_STATUS_ERROR;
    }

    if (HAL_I2C_Mem_Read(&hi2c3, (uint16_t)(INA234_DEV_ADDR << 1), reg,
                          I2C_MEMADD_SIZE_8BIT, dataRx, 2, timeout) != HAL_OK)
    {
        return INA234_STATUS_ERROR;
    }

    *data = ((uint16_t)dataRx[0] << 8) | (uint16_t)dataRx[1];

    return INA234_STATUS_OK;
}

/**
 * @brief   Writes a 16-bit big-endian register over I2C3 (hi2c3), with an
 *          optional read-back verification pass.
 */
static ina234_status_t prvINA234_WriteReg(uint8_t reg,
                                          uint16_t data,
                                          uint8_t writeCheck,
                                          uint32_t timeout)
{
    uint8_t dataTx[2];
    uint16_t readData;

    dataTx[0] = (uint8_t)((data >> 8) & 0xFFU);
    dataTx[1] = (uint8_t)(data & 0xFFU);

    if (HAL_I2C_Mem_Write(&hi2c3, (uint16_t)(INA234_DEV_ADDR << 1), reg,
                           I2C_MEMADD_SIZE_8BIT, dataTx, 2, timeout) != HAL_OK)
    {
        return INA234_STATUS_ERROR;
    }

    if (writeCheck == 1U)
    {
        if (prvINA234_ReadReg(reg, &readData, timeout) != INA234_STATUS_OK)
        {
            return INA234_STATUS_ERROR;
        }

        if (readData != data)
        {
            return INA234_STATUS_ERROR;
        }
    }

    return INA234_STATUS_OK;
}

static ina234_status_t prvINA234_ModifyConfig(uint16_t mask,
                                               uint16_t value,
                                               uint32_t timeout)
{
    uint16_t regData;

    if (prvINA234_ReadReg(INA234_REG_CONFIG, &regData, timeout) != INA234_STATUS_OK)
    {
        return INA234_STATUS_ERROR;
    }

    regData &= (uint16_t)(~mask);
    regData |= (uint16_t)(value & mask);

    return prvINA234_WriteReg(INA234_REG_CONFIG, regData, 1U, timeout);
}

ina234_status_t INA234_Init(const ina234_config_t* config, uint32_t timeout)
{
    if (config == 0)
    {
        return INA234_STATUS_ERROR;
    }

    /* I2C1 is already brought up by MX_I2C1_Init() in main() - no separate
     * instance init needed here (unlike the original DRV_I2C-based driver). */

    if (INA234_Ping(timeout) != INA234_STATUS_OK)
    {
        return INA234_STATUS_ERROR;
    }

    return INA234_SetConfig(config, timeout);
}

ina234_status_t INA234_Ping(uint32_t timeout)
{
    uint16_t manufacturerId;
    uint16_t deviceId;

    if (prvINA234_ReadReg(INA234_REG_MANUFACTURER_ID, &manufacturerId, timeout) != INA234_STATUS_OK)
    {
        return INA234_STATUS_ERROR;
    }

    if (manufacturerId != INA234_MANUFACTURER_ID)
    {
        return INA234_STATUS_ERROR;
    }

    if (prvINA234_ReadReg(INA234_REG_DEVICE_ID, &deviceId, timeout) != INA234_STATUS_OK)
    {
        return INA234_STATUS_ERROR;
    }

    if ((deviceId & 0xFFF0U) != INA234_DEVICE_ID)
    {
        return INA234_STATUS_ERROR;
    }

    return INA234_STATUS_OK;
}

ina234_status_t INA234_Reset(uint32_t timeout)
{
    uint16_t config;

    if (prvINA234_ReadReg(INA234_REG_CONFIG, &config, timeout) != INA234_STATUS_OK)
    {
        return INA234_STATUS_ERROR;
    }

    config |= INA234_CONFIG_RST_MASK;

    /* RST self-clears, therefore read-back verification must not be used here. */
    if (prvINA234_WriteReg(INA234_REG_CONFIG, config, 0U, timeout) != INA234_STATUS_OK)
    {
        return INA234_STATUS_ERROR;
    }

    prvINA234_CurrentLSB = 0.0f;
    prvINA234_ADCRange = INA234_ADC_RANGE_81_92_MV;

    return INA234_STATUS_OK;
}

ina234_status_t INA234_SetConfig(const ina234_config_t* config, uint32_t timeout)
{
    uint16_t regData;

    if (config == 0)
    {
        return INA234_STATUS_ERROR;
    }

    if ((config->range > INA234_ADC_RANGE_20_48_MV) ||
        (config->averaging > INA234_AVG_1024) ||
        (config->busConvTime > INA234_CONV_TIME_8244_US) ||
        (config->shuntConvTime > INA234_CONV_TIME_8244_US) ||
        (config->mode > INA234_MODE_CONTINUOUS_SHUNT_BUS) ||
        (config->shuntResistance <= 0.0f) ||
        (config->currentLSB <= 0.0f))
    {
        return INA234_STATUS_ERROR;
    }

    /* Bits 14:13 are reserved and read back as 10b. */
    regData = 0x4000U;
    regData |= ((uint16_t)config->range << 12);
    regData |= ((uint16_t)config->averaging << 9);
    regData |= ((uint16_t)config->busConvTime << 6);
    regData |= ((uint16_t)config->shuntConvTime << 3);
    regData |= (uint16_t)config->mode;

    if (prvINA234_WriteReg(INA234_REG_CONFIG, regData, 1U, timeout) != INA234_STATUS_OK)
    {
        return INA234_STATUS_ERROR;
    }

    if (INA234_SetCalibration(config->shuntResistance,
                             config->currentLSB,
                             config->range,
                             timeout) != INA234_STATUS_OK)
    {
        return INA234_STATUS_ERROR;
    }

    prvINA234_ADCRange = config->range;

    return INA234_STATUS_OK;
}

ina234_status_t INA234_SetMode(ina234_mode_t mode, uint32_t timeout)
{
    if (mode > INA234_MODE_CONTINUOUS_SHUNT_BUS)
    {
        return INA234_STATUS_ERROR;
    }

    return prvINA234_ModifyConfig(INA234_CONFIG_MODE_MASK, (uint16_t)mode, timeout);
}

ina234_status_t INA234_SetADCRange(ina234_adc_range_t range, uint32_t timeout)
{
    ina234_status_t status;

    if (range > INA234_ADC_RANGE_20_48_MV)
    {
        return INA234_STATUS_ERROR;
    }

    status = prvINA234_ModifyConfig(INA234_CONFIG_ADCRANGE_MASK, ((uint16_t)range << 12), timeout);

    if (status == INA234_STATUS_OK)
    {
        prvINA234_ADCRange = range;
    }

    return status;
}

ina234_status_t INA234_SetAveraging(ina234_avg_t averaging, uint32_t timeout)
{
    if (averaging > INA234_AVG_1024)
    {
        return INA234_STATUS_ERROR;
    }

    return prvINA234_ModifyConfig(INA234_CONFIG_AVG_MASK, ((uint16_t)averaging << 9), timeout);
}

ina234_status_t INA234_SetConversionTime(ina234_conv_time_t shuntTime,
                                         ina234_conv_time_t busTime,
                                         uint32_t timeout)
{
    uint16_t value;

    if ((shuntTime > INA234_CONV_TIME_8244_US) || (busTime > INA234_CONV_TIME_8244_US))
    {
        return INA234_STATUS_ERROR;
    }

    value = ((uint16_t)busTime << 6) | ((uint16_t)shuntTime << 3);

    return prvINA234_ModifyConfig(INA234_CONFIG_VBUSCT_MASK | INA234_CONFIG_VSHCT_MASK, value, timeout);
}

ina234_status_t INA234_SetCalibration(float shuntResistance,
                                      float currentLSB,
                                      ina234_adc_range_t range,
                                      uint32_t timeout)
{
    float calibration;
    uint32_t calibrationCode;

    if ((shuntResistance <= 0.0f) || (currentLSB <= 0.0f) || (range > INA234_ADC_RANGE_20_48_MV))
    {
        return INA234_STATUS_ERROR;
    }

    calibration = INA234_CALIBRATION_CONSTANT / (currentLSB * shuntResistance);

    if (range == INA234_ADC_RANGE_20_48_MV)
    {
        calibration /= 4.0f;
    }

    calibrationCode = (uint32_t)calibration;

    if ((calibrationCode == 0U) || (calibrationCode > 0x7FFFU))
    {
        return INA234_STATUS_ERROR;
    }

    if (prvINA234_WriteReg(INA234_REG_CALIBRATION, (uint16_t)calibrationCode, 1U, timeout) != INA234_STATUS_OK)
    {
        return INA234_STATUS_ERROR;
    }

    prvINA234_CurrentLSB = currentLSB;
    prvINA234_ADCRange = range;

    return INA234_STATUS_OK;
}

ina234_status_t INA234_GetShuntVoltage(float* voltage, uint32_t timeout)
{
    uint16_t regData;
    int16_t value;
    float lsb;

    if (voltage == 0)
    {
        return INA234_STATUS_ERROR;
    }

    if (prvINA234_ReadReg(INA234_REG_SHUNT_VOLTAGE, &regData, timeout) != INA234_STATUS_OK)
    {
        return INA234_STATUS_ERROR;
    }

    value = ((int16_t)regData) >> 4;

    lsb = (prvINA234_ADCRange == INA234_ADC_RANGE_20_48_MV) ? INA234_SHUNT_LSB_RANGE1 : INA234_SHUNT_LSB_RANGE0;

    *voltage = (float)value * lsb;

    return INA234_STATUS_OK;
}

ina234_status_t INA234_GetBusVoltage(float* voltage, uint32_t timeout)
{
    uint16_t regData;
    uint16_t value;

    if (voltage == 0)
    {
        return INA234_STATUS_ERROR;
    }

    if (prvINA234_ReadReg(INA234_REG_BUS_VOLTAGE, &regData, timeout) != INA234_STATUS_OK)
    {
        return INA234_STATUS_ERROR;
    }

    value = (regData >> 4) & 0x07FFU;
    *voltage = (float)value * INA234_BUS_VOLTAGE_LSB;

    return INA234_STATUS_OK;
}

ina234_status_t INA234_GetCurrent(float* current, uint32_t timeout)
{
    uint16_t regData;
    int16_t value;

    if ((current == 0) || (prvINA234_CurrentLSB <= 0.0f))
    {
        return INA234_STATUS_ERROR;
    }

    if (prvINA234_ReadReg(INA234_REG_CURRENT, &regData, timeout) != INA234_STATUS_OK)
    {
        return INA234_STATUS_ERROR;
    }

    value = ((int16_t)regData) >> 4;
    *current = (float)value * prvINA234_CurrentLSB;

    return INA234_STATUS_OK;
}

ina234_status_t INA234_GetPower(float* power, uint32_t timeout)
{
    uint16_t regData;

    if ((power == 0) || (prvINA234_CurrentLSB <= 0.0f))
    {
        return INA234_STATUS_ERROR;
    }

    if (prvINA234_ReadReg(INA234_REG_POWER, &regData, timeout) != INA234_STATUS_OK)
    {
        return INA234_STATUS_ERROR;
    }

    *power = (float)regData * (2.0f * prvINA234_CurrentLSB);

    return INA234_STATUS_OK;
}

ina234_status_t INA234_GetMaskEnable(uint16_t* value, uint32_t timeout)
{
    if (value == 0)
    {
        return INA234_STATUS_ERROR;
    }

    return prvINA234_ReadReg(INA234_REG_MASK_ENABLE, value, timeout);
}

ina234_status_t INA234_SetMaskEnable(uint16_t value, uint32_t timeout)
{
    /* Bits 9:6 and status bits 5:2 are not configuration fields. */
    value &= 0xFC03U;

    return prvINA234_WriteReg(INA234_REG_MASK_ENABLE, value, 1U, timeout);
}

ina234_status_t INA234_GetAlertLimit(uint16_t* value, uint32_t timeout)
{
    if (value == 0)
    {
        return INA234_STATUS_ERROR;
    }

    return prvINA234_ReadReg(INA234_REG_ALERT_LIMIT, value, timeout);
}

ina234_status_t INA234_SetAlertLimit(uint16_t value, uint32_t timeout)
{
    return prvINA234_WriteReg(INA234_REG_ALERT_LIMIT, value, 1U, timeout);
}

ina234_status_t INA234_ReadReg(uint8_t regAddr, uint16_t* data, uint32_t timeout)
{
    return prvINA234_ReadReg(regAddr, data, timeout);
}

ina234_status_t INA234_WriteReg(uint8_t regAddr, uint16_t data, uint32_t timeout)
{
    return prvINA234_WriteReg(regAddr, data, 1U, timeout);
}
