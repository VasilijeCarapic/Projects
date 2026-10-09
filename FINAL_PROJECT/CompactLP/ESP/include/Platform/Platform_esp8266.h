/**
 ******************************************************************************
 * @file    Platform_esp8266.h
 *
 * @brief ESP8266 implementation of the generic platform interface.
 * 
 * Provides the global platform instance and startup functions to allow
 * other firmware components (Source.cpp, CommandParser) to control the 
 * LED and button via Device_Handle().
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026 
 ******************************************************************************
 */
#ifndef PLATFORM_ESP8266_H
#define PLATFORM_ESP8266_H

#include "Platform/Platform.h"

/**
 * @brief	Global ::Platform_t instance wired to the ESP8266 NodeMCU
 * 			hardware (built-in LED on GPIO2, FLASH button on GPIO0).
 * 			Defined in Platform_esp8266.cpp.
 */
extern Platform_t ESP8266_Platform;

/**
 * @brief	Initializes the GPIO pins used by the ESP8266 platform
 * 			(LED pin as output, button pin as input with pull-up).
 * 			Must be called once from setup() before any Device_Handle()
 * 			call is made.
 */
void Platform_ESP8266_Init(void);

#endif