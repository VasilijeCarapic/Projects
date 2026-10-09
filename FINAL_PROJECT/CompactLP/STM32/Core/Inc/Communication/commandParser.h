/**
 ******************************************************************************
 * @file    commandParser.h
 *
 * @brief   Public interface for the top-level command router,
 *          implemented in commandParser.c.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <main.h>
#include <Platform/deviceControl.h>
#include <Communication/transferControl.h>

/**
 * @brief   Top-level entry point for every line received on either UART.
 *          Classifies the message by its "<A> " / "<B>" prefix (single
 *          command vs. multi-line file transfer mode) and dispatches
 *          accordingly.
 * @param   message: NUL-terminated line received from the PC or ESP UART.
 * @param   huart_pc: UART handle for the PC link.
 * @param   huart_esp: UART handle for the ESP link.
 * @retval  None
 */
void CMDPARSE_SortCommand(char *message, UART_HandleTypeDef *huart_pc, UART_HandleTypeDef *huart_esp);


#endif /* COMMAND_PARSER_H */
