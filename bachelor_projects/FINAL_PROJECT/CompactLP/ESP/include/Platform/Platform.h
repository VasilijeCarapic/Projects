/**
 ******************************************************************************
 * @file    Platform.h
 *
 * @brief   Platform abstraction layer. Defines a generic interface 
            that decouples the device control logic
 * 			(device_control.c/.cpp) from the underlying hardware (STM32,
 * 			ESP8266, ...). Each concrete platform (see platform_stm32.c,
 * 			Platform_esp8266.cpp) fills in a ::Platform_t instance with its
 * 			own implementations of these functions, so the same
 * 			Device_Handle() logic can run unmodified on different boards.
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026 
 ******************************************************************************
 */
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

/**
 * @brief	Generic platform interface.
 *
 * 			Collection of function pointers that a concrete platform
 * 			implementation must provide. The device control layer only ever
 * 			calls through this struct, never directly touching hardware
 * 			registers, which keeps it portable.
 */
typedef struct {
    void (*led_on)(void);      /*!< Turns the status LED on */
    void (*led_off)(void);     /*!< Turns the status LED off */
    void (*led_toggle)(void);  /*!< Toggles the current state of the status LED */
    int  (*led_read)(void);    /*!< Reads LED state. @retval 1 = ON, 0 = OFF */
    int  (*btn_read)(void);    /*!< Reads button state. @retval 1 = PRESSED, 0 = not pressed */
    void (*uart_send)(const char *msg); /*!< Sends a null-terminated string back over UART/Serial */
} Platform_t;

#endif