/**
 ******************************************************************************
 * @file    parserTask.c
 *
 * @brief   FreeRTOS task implementation for STM32 firmware:
 *            - prvParserTaskFunction: Receives UART commands and passes them to CMDPARSE_SortCommand().
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */

#include "FreeRTOS.h"
#include "task.h"
#include <Utils/common.h>
#include <Communication/commandParser.h>
#include <Tasks/taskFunctions.h>
#include <main.h>

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;

extern volatile uint8_t rx_byte;
extern volatile char rx_message[100];
extern volatile uint8_t command_ready;
extern char command_buf[100];

extern volatile uint8_t rx_byte_esp;
extern volatile char rx_message_esp[100];
extern volatile uint8_t command_ready_esp;
extern char command_buf_esp[100];

/**
 * @brief Task handle
 */
TaskHandle_t ParserTaskHandle;

/**
 * @brief   Parser task: Waits for incoming UART notifications, copies the
 *          received command safely, and routes it to CMDPARSE_SortCommand().
 *
 * @param   pvParameter: Unused task parameter.
 * @retval  None (Infinite loop).
 */
void prvParserTaskFunction(void *pvParameter)
{
    uint32_t notify_value;
    HAL_UART_Receive_IT(&huart2, (uint8_t*)&rx_byte, 1);
    HAL_UART_Receive_IT(&huart4, (uint8_t*)&rx_byte_esp, 1);

    for(;;)
    {
        xTaskNotifyWait(0, ULONG_MAX, &notify_value, portMAX_DELAY);

        if(!(notify_value & MSG_RECEIVED_FLAG)) continue;

        if (command_ready)
        {
            char command_msg[100];
            strncpy(command_msg, command_buf, sizeof(command_msg) - 1);
            command_msg[sizeof(command_msg) - 1] = '\0';
            command_ready = 0;

            CMDPARSE_SortCommand(command_msg, &huart2, &huart4);
        }

        if (command_ready_esp)
        {
            char command_msg_esp[100];
            strncpy(command_msg_esp, command_buf_esp, sizeof(command_msg_esp) - 1);
            command_msg_esp[sizeof(command_msg_esp) - 1] = '\0';
            command_ready_esp = 0;

            CMDPARSE_SortCommand(command_msg_esp, &huart2, &huart4);
        }
    }
}
