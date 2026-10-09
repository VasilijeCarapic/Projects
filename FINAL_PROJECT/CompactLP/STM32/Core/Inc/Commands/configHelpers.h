/**
 ******************************************************************************
 * @file    configHelpers.h
 * @brief   Helper functions for command parsing and stream verification.
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */

#ifndef CONFIG_HELPERS_H
#define CONFIG_HELPERS_H

#include <stdint.h>
#include "stm32l4xx_hal.h"
#include <Communication/streamConfig.h>

int CFGHLP_ExtractParam(const char *cmd, const char *key, int default_val);
int CFGHLP_ExtractSid(const char *cmd);
Stream_Config_t* CFGHLP_GetStream(int sid);
Stream_Config_t* CFGHLP_StreamErrorCheck(const char *cmd, UART_HandleTypeDef *huart_esp, char *response, uint32_t responseSize);
void CFGHLP_InitLinks(UART_HandleTypeDef *huart_pc);

#endif /* CONFIG_HELPERS_H */
