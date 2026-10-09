/**
 ******************************************************************************
 * @file    Device_control.h
 *
 * @brief   Device control service. Parses text commands received over
 * 			UART/Serial (e.g. "device diode on", "device status button")
 * 			and executes them through the generic ::Platform_t interface,
 * 			so the same command set works unchanged on any supported board
 * 			(STM32, ESP8266, ...).
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026 
 ******************************************************************************
 */
#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include "Platform/Platform.h"

/**
 * @defgroup DEVICE_CONTROL_DEFAULT_PINS Default pin fallbacks
 * @brief	 These defines only apply if the board-specific platform header
 * 			 has not already defined them. On ESP8266, 
 *           LED_PORT/BTN_PORT are unused,
 * 			 only LED_PIN/BTN_PIN matter.
 * @{
 */
#ifndef LED_PORT
#define LED_PORT    GPIOA		/*!< Default LED GPIO port (STM32-style, unused on ESP8266) */
#endif

#ifndef LED_PIN
#define LED_PIN     2			/*!< Default LED GPIO pin number */
#endif
#ifndef BTN_PORT
#define BTN_PORT    GPIOC		/*!< Default button GPIO port (STM32-style, unused on ESP8266) */
#endif

#ifndef BTN_PIN
#define BTN_PIN     0			/*!< Default button GPIO pin number */
#endif
/**
 * @}
 */

/**
 * @brief	Parses and executes a single device command line.
 *
 * 			Recognized commands:
 * 				- "device diode on|off|toggle"
 * 				- "device status diode|button"
 * 			Any response/error message is sent back via platform->uart_send().
 *
 * @param	message: null-terminated command string received from UART
 * 			(e.g. "device diode on"). Only the first 99 characters are used.
 * @param	platform: pointer to the concrete ::Platform_t implementation
 * 			(e.g. &ESP8266_Platform) used to access the LED/button and to
 * 			send responses.
 */
void Device_Handle(const char *message, const Platform_t *platform);

#endif