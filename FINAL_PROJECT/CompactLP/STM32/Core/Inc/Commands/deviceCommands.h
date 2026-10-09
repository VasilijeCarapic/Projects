#ifndef DEVICE_COMMANDS_H
#define DEVICE_COMMANDS_H

#include "stm32l4xx_hal.h"

void DEVCMD_HandleChargerCmds(const char *cmd, UART_HandleTypeDef *huart_esp, char *resp, uint32_t respSz);
void DEVCMD_HandleDacAndSwitchesCmds(const char *cmd, UART_HandleTypeDef *huart_esp, char *resp, uint32_t respSz);
void DEVCMD_HandleWaveAndMiscCmds(const char *cmd, UART_HandleTypeDef *huart_esp, char *resp, uint32_t respSz);

#endif /* DEVICE_COMMANDS_H */
