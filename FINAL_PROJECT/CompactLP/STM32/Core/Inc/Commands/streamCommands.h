#ifndef STREAM_COMMANDS_H
#define STREAM_COMMANDS_H

#include "stm32l4xx_hal.h"
#include <Communication/streamConfig.h>

void STREAMCMD_HandleSlinkCmds(const char *cmd, UART_HandleTypeDef *huart_pc, UART_HandleTypeDef *huart_esp, char *resp, uint32_t respSz);
void STREAMCMD_HandleEplinkCmds(const char *cmd, UART_HandleTypeDef *huart_pc, UART_HandleTypeDef *huart_esp, char *resp, uint32_t respSz);
void STREAMCMD_HandleStreamControlCmds(const char *cmd, UART_HandleTypeDef *huart_esp, char *resp, uint32_t respSz);
void STREAMCMD_HandleAdcCmds(const char *cmd, UART_HandleTypeDef *huart_esp, char *resp, uint32_t respSz);

#endif /* STREAM_COMMANDS_H */
