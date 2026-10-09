/**
 ******************************************************************************
 * @file    platform.h
 *
 * @brief   Generic hardware-abstraction interface (::Platform_t) used by
 *          deviceControl.c so LED/button/UART logic can be reused across
 *          different concrete platform backends (see platformStm32.h/.c
 *          for the STM32 implementation).
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

/**
 * @brief   Set of callbacks a concrete platform must provide to be usable
 *          by deviceControl.c.
 */
typedef struct {
    void (*led_on)(void);              /*!< Turn the status LED on. */
    void (*led_off)(void);             /*!< Turn the status LED off. */
    void (*led_toggle)(void);          /*!< Toggle the status LED. */
    int  (*led_read)(void);            /*!< Read the LED state: 1 = on, 0 = off. */
    int  (*btn_read)(void);            /*!< Read the button state: 1 = pressed, 0 = not pressed. */
    void (*uart_send)(const char *msg);/*!< Send a NUL-terminated message over UART. */
} Platform_t;

#endif
