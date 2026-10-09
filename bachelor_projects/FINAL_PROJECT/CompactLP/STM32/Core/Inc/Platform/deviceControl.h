/**
 ******************************************************************************
 * @file    deviceControl.h
 *
 * @brief   Public interface for platform "device" (LED/button)
 *          command handling, implemented in deviceControl.c. Also
 *          provides fallback default LED/button pin definitions used when
 *          main.h has not already defined them.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include <Platform/platform.h>


#ifndef LED_PORT
#define LED_PORT    GPIOA
#endif

#ifndef LED_PIN
#define LED_PIN     GPIO_PIN_5
#endif

#ifndef BTN_PORT
#define BTN_PORT    GPIOC
#endif

#ifndef BTN_PIN
#define BTN_PIN     GPIO_PIN_13
#endif


/**
 * @brief   Entry point for "device" commands ("show device diode ...",
 *          "show device status ...").
 * @param   message: full raw command line.
 * @param   platform: platform instance (LED/button/UART callbacks) used to
 *          execute the command and reply.
 * @retval  None
 */
void DEVCTRL_Handle(const char *message, const Platform_t *platform);

#endif
