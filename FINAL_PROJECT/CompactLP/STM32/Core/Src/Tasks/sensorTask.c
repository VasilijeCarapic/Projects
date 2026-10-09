/**
 ******************************************************************************
 * @file    sensorTask.c
 *
 * @brief   FreeRTOS task implementation for STM32 firmware:
 *            - prvSensorTaskFunction: Handles I2C operations and periodic streaming flags.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */

#include "FreeRTOS.h"
#include "task.h"
#include <Utils/common.h>
#include <Communication/commandParser.h>
#include <Communication/streamConfig.h>
#include <Tasks/taskFunctions.h>
#include <main.h>

void I2C_Scan(void);

extern UART_HandleTypeDef huart4;

/**
 * @brief Task handle
 */
TaskHandle_t SensorTaskHandle;
device_acquisition_type_t SENSOR_TASK_ACQUISITION_TYPE = DEVICE_ACQUISITION_INT;

static uint32_t prvSENSORTASK_ConstructByteMessage(device_acquisition_type_t acq_type, uint8_t frameBuf[FRAME_LEN],
        int id, uint32_t packetCounter[SSTREAM_CONNECTIONS_MAX_NUMBER]);

/**
 * @brief   Sensor task: builds one binary stream frame per active stream
 *          every 15ms, containing samplesNo interleaved (INT) or blocked (EXT)
 *          voltage/current int16 samples, and sends it to the ESP via the
 *          binary UART path.
 */
void prvSensorTaskFunction(void *pvParameter)
{
    TickType_t last_send_time[SSTREAM_CONNECTIONS_MAX_NUMBER] = {0};
    uint32_t   packetCounter[SSTREAM_CONNECTIONS_MAX_NUMBER]  = {0};

    static uint8_t frameBuf[FRAME_LEN];

    device_acquisition_type_t acq_type = SENSOR_TASK_ACQUISITION_TYPE;

    for (;;)
    {
        uint8_t active_streams = 0;
        TickType_t current_time = xTaskGetTickCount();

        for (int i = 0; i < SSTREAM_CONNECTIONS_MAX_NUMBER; i++)
        {
            if (streams[i].in_use && streams[i].is_streaming)
            {
                active_streams++;

                if ((current_time - last_send_time[i]) >= SENSOR_TASK_DELAY)
                {
                    uint32_t payloadLen = prvSENSORTASK_ConstructByteMessage(acq_type, frameBuf, i, packetCounter);

                    UART_SendBinary(&huart4, frameBuf, SYNQ_BYTES + payloadLen);

                    last_send_time[i] = current_time;
                }
            }
            else
            {
                last_send_time[i] = 0;
                packetCounter[i] = 0;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(active_streams > 0 ? 15 : 500));
    }
}

/**
 * @brief   Packs byte message in accordance to acq_type EXT / INT.
 *          Payload length now scales with the actual samplesNo of the
 *          stream (clamped to SAMPLE_SIZE), not with the fixed FRAME_LEN
 *          buffer size. FRAME_LEN is only the worst-case buffer size.
 *
 * @retval  Actual payload length in bytes (MAGIC_CTR_BYTES + sample bytes),
 *          i.e. everything after the 4-byte sync/id/len header.
 */
static uint32_t prvSENSORTASK_ConstructByteMessage(device_acquisition_type_t acq_type, uint8_t frameBuf[FRAME_LEN],
	int id, uint32_t packetCounter[SSTREAM_CONNECTIONS_MAX_NUMBER])
{
	uint32_t samplesNo  = streams[id].adc_samplesno;
	uint32_t magic      = 0;
	uint32_t counter    = 0;

	if (samplesNo == 0) samplesNo = 1;
	if (samplesNo > SAMPLE_SIZE) samplesNo = SAMPLE_SIZE; /* clamp to buffer */

	uint32_t payloadLen = MAGIC_CTR_BYTES + samplesNo * CURRENT_BYTES * VOLTAGE_BYTES;

	/* same is done for both of the modes 4 + 8 bytes first are the same */
	frameBuf[0] = 0xAA;
	frameBuf[1] = (uint8_t)id;
	frameBuf[2] = (uint8_t)(payloadLen & 0xFF);
	frameBuf[3] = (uint8_t)((payloadLen >> 8) & 0xFF);

	counter = packetCounter[id]++;
	magic = 0; /* upper 16 bita = EBP flag; 0 = does not have EBP flag */

	memcpy(&frameBuf[4], &counter, 4);
	memcpy(&frameBuf[8], &magic, 4);

	int16_t *samplePtr = (int16_t*)&frameBuf[12];

	switch(acq_type){
	case DEVICE_ACQUISITION_INT:
		/* interleaved: V0,I0,V1,I1,... */
		for (uint32_t s = 0; s < samplesNo; s++)
		{
			samplePtr[s*2]     = (int16_t)Sensor_ReadVoltage();
			samplePtr[s*2 + 1] = (int16_t)Sensor_ReadCurrent();
		}
		break;

	case DEVICE_ACQUISITION_EXT:
		/* blocked: I0..In, V0..Vn */
		for (uint32_t s = 0; s < samplesNo; s++)
		{
			samplePtr[s]             = (int16_t)Sensor_ReadCurrent();
			samplePtr[samplesNo + s] = (int16_t)Sensor_ReadVoltage();

			/* byte swap */
			uint16_t cur = (uint16_t)samplePtr[s];
			uint16_t vol = (uint16_t)samplePtr[samplesNo + s];

			samplePtr[s]             = (int16_t)((cur << 8) | (cur >> 8));
			samplePtr[samplesNo + s] = (int16_t)((vol << 8) | (vol >> 8));
		}
		break;

	default:
		break;
	}

	return payloadLen;
}
