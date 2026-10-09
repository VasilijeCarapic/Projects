/**
 ******************************************************************************
 * @file    ina234.h
 *
 * @brief   INA234 current, voltage and power monitor driver.
 *
 *          Ported from Haris Turkmanovic's original CLP-master driver
 *          (Core/HAL/INA234A) to talk to the STM32 HAL I2C1 peripheral
 *          (hi2c3) directly, instead of his DRV_I2C abstraction layer. The
 *          public API and register-level logic are unchanged.
 *
 * @authors Haris Turkmanovic (original), Veljko Sokolov (HAL I2C1 port)
 * @date    September 2026
 ******************************************************************************
 */

#ifndef SENSORS_INA234_H_
#define SENSORS_INA234_H_

#include <stdint.h>

/**
 * @brief INA234 driver return status
 */
typedef enum
{
    INA234_STATUS_OK = 0,
    INA234_STATUS_ERROR
} ina234_status_t;

/**
 * @brief INA234 shunt ADC input range
 */
typedef enum
{
    INA234_ADC_RANGE_81_92_MV = 0,
    INA234_ADC_RANGE_20_48_MV = 1
} ina234_adc_range_t;

/**
 * @brief INA234 ADC averaging configuration
 */
typedef enum
{
    INA234_AVG_1 = 0,
    INA234_AVG_4,
    INA234_AVG_16,
    INA234_AVG_64,
    INA234_AVG_128,
    INA234_AVG_256,
    INA234_AVG_512,
    INA234_AVG_1024
} ina234_avg_t;

/**
 * @brief INA234 ADC conversion time
 */
typedef enum
{
    INA234_CONV_TIME_140_US = 0,
    INA234_CONV_TIME_204_US,
    INA234_CONV_TIME_332_US,
    INA234_CONV_TIME_588_US,
    INA234_CONV_TIME_1100_US,
    INA234_CONV_TIME_2116_US,
    INA234_CONV_TIME_4156_US,
    INA234_CONV_TIME_8244_US
} ina234_conv_time_t;

/**
 * @brief INA234 operating mode
 */
typedef enum
{
    INA234_MODE_SHUTDOWN = 0,
    INA234_MODE_TRIGGERED_SHUNT = 1,
    INA234_MODE_TRIGGERED_BUS = 2,
    INA234_MODE_TRIGGERED_SHUNT_BUS = 3,
    INA234_MODE_SHUTDOWN_ALT = 4,
    INA234_MODE_CONTINUOUS_SHUNT = 5,
    INA234_MODE_CONTINUOUS_BUS = 6,
    INA234_MODE_CONTINUOUS_SHUNT_BUS = 7
} ina234_mode_t;

/**
 * @brief INA234 initialization/configuration structure
 */
typedef struct
{
    ina234_adc_range_t range;
    ina234_avg_t averaging;
    ina234_conv_time_t busConvTime;
    ina234_conv_time_t shuntConvTime;
    ina234_mode_t mode;
    float shuntResistance;     /*!< Shunt resistance in ohms */
    float currentLSB;          /*!< Current register LSB in amperes */
} ina234_config_t;

ina234_status_t INA234_Init(const ina234_config_t* config, uint32_t timeout);
ina234_status_t INA234_Ping(uint32_t timeout);
ina234_status_t INA234_Reset(uint32_t timeout);

ina234_status_t INA234_SetConfig(const ina234_config_t* config, uint32_t timeout);
ina234_status_t INA234_SetMode(ina234_mode_t mode, uint32_t timeout);
ina234_status_t INA234_SetADCRange(ina234_adc_range_t range, uint32_t timeout);
ina234_status_t INA234_SetAveraging(ina234_avg_t averaging, uint32_t timeout);
ina234_status_t INA234_SetConversionTime(ina234_conv_time_t shuntTime,
                                         ina234_conv_time_t busTime,
                                         uint32_t timeout);
ina234_status_t INA234_SetCalibration(float shuntResistance,
                                      float currentLSB,
                                      ina234_adc_range_t range,
                                      uint32_t timeout);

ina234_status_t INA234_GetShuntVoltage(float* voltage, uint32_t timeout);
ina234_status_t INA234_GetBusVoltage(float* voltage, uint32_t timeout);
ina234_status_t INA234_GetCurrent(float* current, uint32_t timeout);
ina234_status_t INA234_GetPower(float* power, uint32_t timeout);

ina234_status_t INA234_GetMaskEnable(uint16_t* value, uint32_t timeout);
ina234_status_t INA234_SetMaskEnable(uint16_t value, uint32_t timeout);
ina234_status_t INA234_GetAlertLimit(uint16_t* value, uint32_t timeout);
ina234_status_t INA234_SetAlertLimit(uint16_t value, uint32_t timeout);

ina234_status_t INA234_ReadReg(uint8_t regAddr, uint16_t* data, uint32_t timeout);
ina234_status_t INA234_WriteReg(uint8_t regAddr, uint16_t data, uint32_t timeout);

#endif /* SENSORS_INA234_H_ */
