/**
 ******************************************************************************
 * @file    usefulFunctions.c
 *
 * @brief   Implementation of the UART transmit helper layer. Provides the
 *          UTILFN_GenerateData() s(check: type of messages) and the UART_Send*()
 *          family of functions, which wrap outgoing messages into a
 *          ::TxMsg_t and push them into the high/low priority TX queues
 *          (qTxHigh / qTxLow) consumed by prvUartTxTaskFunction().
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include <Communication/txRouter.h>
#include <Tasks/taskFunctions.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <Utils/usefulFunctions.h>
#include <Sensors/ina234.h>


extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;
extern SemaphoreHandle_t uartTxGuard;
extern volatile uint8_t ina234Ready;

/**
 * @brief   Generates a pseudo-random temperature/pressure reading and sends
 *          it to the ESP as a formatted "check" response.
 *
 *          Temperature is generated in the range [-10, 80] C and pressure
 *          in the range [1, 1200] bar (see inline comments for the exact
 *          formulas), formatted into a single line and transmitted via
 *          UART_Send() on the low-priority queue.
 *
 * @param   huart_esp: destination UART handle (normally &huart4, the ESP
 *          link) that the generated line is sent to.
 * @retval  None
 */
void UTILFN_GenerateData(UART_HandleTypeDef *huart_esp)
{
	float temperature = -10 + (rand() % 901)/10.0;  /* TEMPERATURE FROM -10C to 80C */
	float pressure = 1.0 + (rand() % 1200)/1.0;  	/* PRESSURE FROM 1bar to 40var */

	char send[100];
	snprintf(send, sizeof(send), "Check: [T = %.1f C; p = %.1f bar;]\r\n", temperature, pressure);
	UART_Send(huart_esp, send);
}

/**
 * @brief   Button press handler, intended to be called from the button's
 *          EXTI interrupt callback.
 *
 *          Applies a simple 50 ms software debounce based on HAL_GetTick():
 *          if less than 50 ms have elapsed since the last accepted press,
 *          the call is ignored. Otherwise, check: temp = ..., pressure = ... is send to ESP.
 *
 * @param   huart: destination UART handle passed through to UTILFN_GenerateData()
 *          (typically the ESP ).
 * @retval  None
 */
void UTILFN_ButtonPressed(UART_HandleTypeDef *huart)
{
    static uint32_t last_tick = 0;

    uint32_t now = HAL_GetTick();

    if ((now - last_tick) < 50)
    {
        return;
    }

    last_tick = now;

    UTILFN_GenerateData(huart);
}

/**
 * @brief   Refreshes the cached INA234 voltage/current at most once every
 *          INA234_CACHE_INTERVAL_MS. sensorTask.c calls Sensor_ReadVoltage()/
 *          Sensor_ReadCurrent() once PER SAMPLE (up to adc_samplesno = 250
 *          times per packet) - doing a real blocking I2C transaction on
 *          every single call would stall SensorTask for seconds per packet
 *          and starve the whole stream. Caching bounds the I2C traffic to a
 *          fixed rate regardless of samplesNo.
 */
#define INA234_CACHE_INTERVAL_MS 10U

static uint32_t prvIna234LastReadTick = 0;
static float    prvIna234CachedVoltage = 0.0f;
static float    prvIna234CachedCurrent = 0.0f;

static void prvIna234RefreshCache(void)
{
    uint32_t now = HAL_GetTick();
    float value;

    if ((now - prvIna234LastReadTick) < INA234_CACHE_INTERVAL_MS) return;
    prvIna234LastReadTick = now;

    if (INA234_GetBusVoltage(&value, 20U) == INA234_STATUS_OK)
    {
        prvIna234CachedVoltage = value;
    }
    if (INA234_GetCurrent(&value, 20U) == INA234_STATUS_OK)
    {
        prvIna234CachedCurrent = value;
    }
}

/**
 * @brief   Mirrors GUI-master/Source/Processing/dataprocessing.h's DEFAULT
 *          calibration constants (DATAPROCESSING_DEFAULT_*), i.e. what the
 *          GUI's Calibration window shows unless the user has changed it.
 *          The GUI descales every raw sample it receives as:
 *            voltageInc  = adcVoltageRef/2^resolution*voltageCorr
 *            currentInc  = adcVoltageRef/2^resolution
 *            voltageValue = voltageOff + raw*voltageInc
 *            currentValue(mA) = (raw*currentInc - voltageCurrOffset)
 *                                /(currentShunt*currentGain)*1000*currentCorrection
 *          (see dataprocessing.cpp onNewSampleBufferReceived()). We invert
 *          that here so a REAL INA234 reading (V, A) survives the GUI's
 *          descaling and comes out physically correct on the other end -
 *          this is also exactly the convention the old dummy generator's
 *          18000-20000 raw range and "1.63V offset" comment were tuned for.
 */
#define GUI_CAL_ADC_VOLTAGE_REF     8.179f
#define GUI_CAL_ADC_VOLTAGE_K       1.3348f
#define GUI_CAL_ADC_VOLTAGE_OFF     0.0f
#define GUI_CAL_CURRENT_SHUNT       0.100f
#define GUI_CAL_CURRENT_GAIN        9.38f
#define GUI_CAL_CURRENT_K           1.0f
#define GUI_CAL_CURRENT_OFF         1.6302f
#define GUI_CAL_ADC_RESOLUTION_BITS 16

static int32_t prvIna234ScaleVoltage(float busVoltageV)
{
    float voltageInc = (GUI_CAL_ADC_VOLTAGE_REF / (float)(1UL << GUI_CAL_ADC_RESOLUTION_BITS)) * GUI_CAL_ADC_VOLTAGE_K;
    float raw = (busVoltageV - GUI_CAL_ADC_VOLTAGE_OFF) / voltageInc;
    return (int32_t)raw;
}

static int32_t prvIna234ScaleCurrent(float currentA)
{
    float currentInc = GUI_CAL_ADC_VOLTAGE_REF / (float)(1UL << GUI_CAL_ADC_RESOLUTION_BITS);
    float currentMA  = currentA * 1000.0f;
    float raw = (GUI_CAL_CURRENT_OFF + currentMA * (GUI_CAL_CURRENT_SHUNT * GUI_CAL_CURRENT_GAIN)
                / (1000.0f * GUI_CAL_CURRENT_K)) / currentInc;
    return (int32_t)raw;
}

/**
 * @brief   Reads bus voltage from the INA234 (real sensor path, cached -
 *          see prvIna234RefreshCache()) if it was successfully initialized
 *          at startup (see ina234Ready, set in main()'s USER CODE 2);
 *          otherwise falls back to the original dummy/simulated value so
 *          the rest of the pipeline (packet format, INT/EXT mode,
 *          streaming) is completely unaffected. The real reading is scaled
 *          into the GUI's raw ADC-code convention (see
 *          prvIna234ScaleVoltage()) before being cast to int16_t downstream.
 * @retval  Scaled raw voltage code (real reading), or a dummy raw value in
 *          the 18000-19999 range (unchanged fallback behavior).
 */
uint32_t Sensor_ReadVoltage(void)
{
    if (ina234Ready)
    {
        prvIna234RefreshCache();
        return (uint32_t)prvIna234ScaleVoltage(prvIna234CachedVoltage);
    }

    return 18000 + (rand() % 2000);
}

/**
 * @brief   Reads current from the INA234 (real sensor path, cached - see
 *          prvIna234RefreshCache()) if it was successfully initialized at
 *          startup; otherwise falls back to the original dummy/simulated
 *          value. See Sensor_ReadVoltage() for the fallback/scaling
 *          rationale.
 * @retval  Scaled raw current code (real reading), or a dummy raw value in
 *          the 18000-19999 range (unchanged fallback behavior).
 */
uint32_t Sensor_ReadCurrent(void)
{
    if (ina234Ready)
    {
        prvIna234RefreshCache();
        return (uint32_t)prvIna234ScaleCurrent(prvIna234CachedCurrent);
    }

    /* Simulate raw ADC current values (~18000–20000) to keep the voltage above the 1.63V offset and yield positive mA values */
    return 18000 + (rand() % 2000);
}


/**
 * @brief   Function that finds on which destination we send the data,
 *          esp/pc/to both.
 * @param   huart: pointer to the current channel. NULL -> both, &huart4 ->
 *          ESP, &huart2 -> PC.
 * @retval  ::TxDest_t destination to which the message should be routed
 *          (TX_DEST_BOTH, TX_DEST_ESP or TX_DEST_PC). Any handle that is
 *          neither NULL, &huart4 nor &huart2 falls back to TX_DEST_PC.
 */
static TxDest_t prvUTILFN_GetDest(UART_HandleTypeDef* huart){

	if(huart == NULL){ 
		return TX_DEST_BOTH;
	}
	else if(huart == &huart4){
		return TX_DEST_ESP; 
	}
	else if(huart == &huart2){
		return TX_DEST_PC; 
	}
	else{
		return TX_DEST_PC;
	}

}

/**
 * @brief   Stores a message in a queue depending on what priority is the
 *          priority of the task, and notifies the UART TX task that a new
 *          message is waiting.
 *
 *          Builds a ::TxMsg_t (truncated/NUL-terminated to fit tx.data),
 *          resolves the destination via prvUTILFN_GetDest(), pushes it onto the
 *          given queue, and raises FLAG_HP_QUEUE or FLAG_LP_QUEUE on
 *          UartTxTaskHandle depending on whether qTxHigh or another queue
 *          was targeted.
 *
 * @param   huart: destination UART handle, or NULL to broadcast to both
 *          (see prvUTILFN_GetDest()). Note: huart is allowed to be NULL - only queue
 *          and msg are validated.
 * @param   msg: NUL-terminated message to send.
 * @param   queue: target FreeRTOS queue (qTxHigh or qTxLow).
 * @retval  None
 */
void UART_Send_Prototype(UART_HandleTypeDef *huart, const char *msg, 
						QueueHandle_t queue)
{
	/* Proveravamo samo queue i msg - huart SME da bude NULL kad se salje na oba! */
	if (queue == NULL || msg == NULL) return;

	TxMsg_t tx; 

	tx.len = strnlen(msg, sizeof(tx.data) - 1);
	memcpy(tx.data, msg, tx.len);
	tx.data[tx.len] = '\0'; 

	tx.dest = prvUTILFN_GetDest(huart); 

	xQueueSendToBack(queue, &tx, portMAX_DELAY);

	if (queue == qTxHigh) 
	{ 
		xTaskNotify(UartTxTaskHandle, FLAG_HP_QUEUE, eSetBits);
	}
	else                  
	{ 
		xTaskNotify(UartTxTaskHandle, FLAG_LP_QUEUE, eSetBits);
	}
}

/**
 * @brief sends binary data to the STM. transmit is protected with binary semaphore so that transmit
 * cannot be intercepted.
 * */
void UART_SendBinary(UART_HandleTypeDef *huart, const uint8_t *data, uint16_t len)
{
	if (xSemaphoreTake(uartTxGuard, portMAX_DELAY) != pdTRUE) return;
	HAL_UART_Transmit(huart, (uint8_t*)data, len, 100);
	xSemaphoreGive(uartTxGuard);
}

/**
 * @brief   Stores a msg in the high priority queue and notifies the
 *          Transfer task.
 * @param   huart: destination UART handle (or NULL for both).
 * @param   msg: NUL-terminated message to send.
 * @retval  None
 */
void UART_SendHP(UART_HandleTypeDef* huart, const char* msg)
{
	UART_Send_Prototype(huart, msg, qTxHigh);
}

/**
 * @brief   Stores a msg in the low priority queue and notifies the
 *          Transfer task.
 * @param   huart: destination UART handle (or NULL for both).
 * @param   msg: NUL-terminated message to send.
 * @retval  None
 */
void UART_Send(UART_HandleTypeDef *huart, const char* msg)
{
	UART_Send_Prototype(huart, msg, qTxLow);
}

/**
 * @brief   Sends to both (PC i ESP) in Low Priority Queue.
 * @param   msg: NUL-terminated message to broadcast to both UARTs.
 * @retval  None
 */
void UART_SendToBoth(const char *msg)
{
	UART_Send_Prototype(NULL, msg, qTxLow);
}

/**
 * @brief   Sends to both (PC i ESP) in High Priority Queue.
 * @param   msg: NUL-terminated message to broadcast to both UARTs.
 * @retval  None
 */
void UART_SendToBothHP(const char *msg)
{
	UART_Send_Prototype(NULL, msg, qTxHigh);
}



