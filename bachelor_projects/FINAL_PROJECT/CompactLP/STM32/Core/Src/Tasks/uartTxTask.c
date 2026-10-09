/**
 ******************************************************************************
 * @file    uartTxTask.c
 *
 * @brief   FreeRTOS task implementation for STM32 firmware:
 *            - prvUartTxTaskFunction: Sends high and low priority TX queues over UART.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */

#include "FreeRTOS.h"
#include "task.h"
#include <Utils/common.h>
#include <Tasks/taskFunctions.h>
#include <Communication/txRouter.h>
#include <main.h>

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;

/* Queue handles */
extern QueueHandle_t  qTxHigh;
extern QueueHandle_t  qTxLow;
extern SemaphoreHandle_t uartTxGuard;

/**
 * @brief Task handle
 */
TaskHandle_t UartTxTaskHandle;

static void prvUARTTX_Transmit(TxMsg_t* tx_msg);

/**
 * @brief   UART TX task: Sends high-priority messages first, then low-priority ones.
 *
 * @param   pvParameter: Unused task parameter.
 * @retval  None (Infinite loop).
 */
void prvUartTxTaskFunction(void *pvParameter)
{
    TxMsg_t msg;
    uint32_t notify_value;
    for (;;)
    {
    	xTaskNotifyWait(0, ULONG_MAX, &notify_value, portMAX_DELAY );

        if(notify_value & FLAG_HP_QUEUE){
        	while (xQueueReceive(qTxHigh, &msg, 0) == pdTRUE)
        	{
        		prvUARTTX_Transmit(&msg);
        	}
        }

        if(notify_value & FLAG_LP_QUEUE){
        	while (xQueueReceive(qTxLow, &msg, 0) == pdTRUE)
			{
				prvUARTTX_Transmit(&msg);
			}

        }
    }
}

/**
 * @brief   Sends a message over UART to the specified destination (PC, ESP, or both).
 * @param   tx_msg: Pointer to the TX message structure.
 * @retval  None
 */
static void prvUARTTX_Transmit(TxMsg_t* tx_msg)
{
	if (xSemaphoreTake(uartTxGuard, portMAX_DELAY) != pdTRUE) return;

	if (tx_msg->dest == TX_DEST_ESP)
	{
		HAL_UART_Transmit(&huart4, (uint8_t*)tx_msg->data, tx_msg->len, 100);
	}
	else if (tx_msg->dest == TX_DEST_PC)
	{
		HAL_UART_Transmit(&huart2, (uint8_t*)tx_msg->data, tx_msg->len, 100);
	}
	else if (tx_msg->dest == TX_DEST_BOTH)
	{
		HAL_UART_Transmit(&huart4, (uint8_t*)tx_msg->data, tx_msg->len, 100);
		HAL_UART_Transmit(&huart2, (uint8_t*)tx_msg->data, tx_msg->len, 100);
	}

	xSemaphoreGive(uartTxGuard);
}
