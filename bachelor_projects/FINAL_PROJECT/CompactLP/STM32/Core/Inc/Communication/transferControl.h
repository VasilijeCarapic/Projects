/**
 ******************************************************************************
 * @file    transferControl.h
 *
 * @brief   Public interface for the "transmit" command family (check /
 *          behavior / value / clear / infomsg1-3), implemented in
 *          transferControl.c.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef TRANSFER_CONTROL_H
#define TRANSFER_CONTROL_H

#include <main.h>

/**
 * @brief   Entry point for all "transmit ..." commands.
 * @param   message: full raw command line, e.g. "transmit value=\"...\"".
 * @param   huart_pc: UART handle used for status/error replies to the PC.
 * @param   huart_esp: UART handle used to forward payloads to the ESP.
 * @retval  None
 */
void TRANSFERCTRL_Handle(const char *message, UART_HandleTypeDef *huart_pc, UART_HandleTypeDef *huart_esp);

#endif /* TRANSFER_CONTROL_H */
