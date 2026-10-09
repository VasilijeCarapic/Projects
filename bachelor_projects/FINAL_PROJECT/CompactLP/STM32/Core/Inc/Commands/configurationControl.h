/**
 ******************************************************************************
 * @file    configurationControl.h
 *
 * @brief   Public interface for the "device"/"charger"/"stream"/"eplink"/
 *          "slink" command family, implemented in configurationControl.c.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef CONFIGURATION_CONTROL_H
#define CONFIGURATION_CONTROL_H

#include <Utils/usefulFunctions.h>
#include <main.h>
#include "stm32l4xx_hal.h"

/**
 * @brief   Entry point for all "device"/"charger"/"stream"/"eplink"/
 *          "slink" commands.
 * @param   cmd: full raw command line.
 * @param   huart_pc: UART handle used for debug echoes and human-readable
 *          error messages.Sends data to the pc.
 * @param   huart_esp: UART handle used for the "OK"/"ERROR" protocol
 *          replies.Sends data to the ESP.
 * @retval  None
 */
void CONFCTRL_Handle(const char *cmd, UART_HandleTypeDef *huart_pc, UART_HandleTypeDef *huart_esp);

#endif
