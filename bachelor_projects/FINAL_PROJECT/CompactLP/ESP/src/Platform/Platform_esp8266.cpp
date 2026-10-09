/**
 ******************************************************************************
 * @file    Platform_esp8266.cpp
 *
 * @brief   ESP8266 NodeMCU implementation of the generic ::Platform_t
 * 			interface declared in platform.h. Provides LED/button access
 * 			and UART responses so that the shared Device_Handle() logic
 * 			(device_control.cpp) can run unmodified on this board.
 *
 *
 * @authors	Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026 
 ******************************************************************************
 */

#include <Arduino.h>
#include "Platform/Platform.h"
#include "Platform/Platform_esp8266.h"
#include "Platform/Device_control.h"


/**
 * @brief	Turns the built-in LED on.
 *
 * 			NodeMCU's built-in LED is ACTIVE LOW:
 * 				digitalWrite(pin, LOW)  = LED ON
 * 				digitalWrite(pin, HIGH) = LED OFF
 * 			Opposite of the STM32 where GPIO_PIN_SET = on.
 * 			This is a platform-internal detail - device_control.c doesn't
 * 			know about it, it just calls led_on() and expects the LED
 * 			to turn on.
 */
static void esp_led_on(void)
{
    digitalWrite(LED_PIN, LOW);
}

/**
 * @brief	Turns the built-in LED off (HIGH on this active-low LED).
 */
static void esp_led_off(void)
{
    digitalWrite(LED_PIN, HIGH);  /* HIGH = ugaseno na active LOW LED-u */
}

/**
 * @brief	Toggles the current LED state.
 *
 * 			digitalRead() returns the pin's current state (LOW or HIGH).
 * 			'!' inverts it: LOW (on) -> HIGH (turns off),
 * 			                HIGH (off) -> LOW (turns on).
 */
static void esp_led_toggle(void)
{
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}

/**
 * @brief	Reads the current LED state.
 *
 * 			Returns 1 if the LED is ON, 0 if OFF. Since the LED is
 * 			active-low (LOW = on, HIGH = off), the raw pin read is
 * 			inverted: LOW -> 1, HIGH -> 0.
 * @retval	1 if LED is on, 0 if off.
 */
static int esp_led_read(void)
{
    return digitalRead(LED_PIN) == LOW ? 1 : 0;
}

/**
 * @brief	Reads the current FLASH button state.
 *
 * 			The button is active-low: pressed = LOW. Returns 1 if PRESSED.
 * @retval	1 if the button is pressed, 0 otherwise.
 */
static int esp_btn_read(void)
{
    return digitalRead(BTN_PIN) == LOW ? 1 : 0;
}

/**
 * @brief	Sends a response string back over the same UART used to
 * 			receive commands from the STM32.
 *
 * 			Serial is the same UART that receives commands from the STM32;
 * 			responses ("LED is ON", etc.) go back to the STM32, which then
 * 			forwards them to the PC as debug info.
 * @param	msg: null-terminated string to send.
 */
static void esp_uart_send(const char *msg)
{
    Serial.print(msg);
}

/**
 * @brief	Global ::Platform_t instance for the ESP8266 NodeMCU, wiring
 * 			the generic platform interface to the static functions defined
 * 			above. Same layout as STM32_Platform in platform_stm32.c.
 */
Platform_t ESP8266_Platform = {
    .led_on     = esp_led_on,
    .led_off    = esp_led_off,
    .led_toggle = esp_led_toggle,
    .led_read   = esp_led_read,
    .btn_read   = esp_btn_read,
    .uart_send  = esp_uart_send,
};

/**
 * @brief	Initializes the GPIO pins used by this platform: LED pin as
 * 			output (starting OFF, i.e. HIGH since active-low), and button
 * 			pin as input with the internal pull-up enabled.
 * 			Must be called once from setup().
 */
void Platform_ESP8266_Init(void)
{
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);  

    pinMode(BTN_PIN, INPUT_PULLUP); 
}

