/**
 ******************************************************************************
 * @file    platformStm32.h
 *
 * @brief   STM32 backend of the generic ::Platform_t interface (see
 *          platform.h), implemented in platformStm32.c.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef PLATFORM_STM32_H
#define PLATFORM_STM32_H

#include <Platform/platform.h>
#include <main.h>

/**
 * @brief   Selects which UART instance the STM32 platform's uart_send
 *          callback transmits on. Call once in main() before entering the
 *          scheduler.
 * @param   huart: UART handle to bind.
 * @retval  None
 */
void Platform_STM32_Init(UART_HandleTypeDef *huart);

/* GLOBAL INSTANCE - USED BY commandParser.c */
extern Platform_t STM32_Platform;

#endif
