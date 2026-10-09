/**
 ******************************************************************************
 * @file    platformStm32.c
 *
 * @brief   STM32 implementation of ::Platform_t.
 *        Maps high-level control logic (LED, button, UART) directly
 *        to STM32 HAL calls.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include <Platform/platformStm32.h>
#include <Platform/deviceControl.h>   /* LED_PORT, LED_PIN, BTN_PORT, BTN_PIN */
#include <string.h>

/* Which UART is used  - is set through Platform_STM32_Init() */
static UART_HandleTypeDef *s_huart = NULL;


/**
 * @brief   Turns the status LED on.
 * @retval  None
 */
static void stm32_led_on(void)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
}

/**
 * @brief   Turns the status LED off.
 * @retval  None
 */
static void stm32_led_off(void)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
}

/**
 * @brief   Toggles the status LED.
 * @retval  None
 */
static void stm32_led_toggle(void)
{
    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
}

/**
 * @brief   Reads back the current state of the status LED.
 * @retval  1 if the LED pin is set (LED on), 0 otherwise.
 */
static int stm32_led_read(void)
{
    return (HAL_GPIO_ReadPin(LED_PORT, LED_PIN) == GPIO_PIN_SET) ? 1 : 0;
}

/**
 * @brief   Reads the state of the user button.
 * @note    The button input is active-low, so a GPIO_PIN_RESET reading is
 *          reported as "pressed".
 * @retval  1 if the button is pressed, 0 otherwise.
 */
static int stm32_btn_read(void)
{
    return (HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) == GPIO_PIN_RESET) ? 1 : 0;
}

/**
 * @brief   Sends a NUL-terminated string over the UART configured via
 *          Platform_STM32_Init(), using a blocking HAL_UART_Transmit()
 *          call.
 * @param   msg: NUL-terminated message to transmit.
 * @retval  None. If Platform_STM32_Init() has not been called yet
 *          (s_huart == NULL), the call is silently ignored.
 */
static void stm32_uart_send(const char *msg)
{
    if (s_huart == NULL) return;
    UART_Send(s_huart, msg);
}


/**
 * @brief
 * STM32 ::Platform_t instance, c
 * connecting low-level hardware functions to the generic platform interface.
 */

Platform_t STM32_Platform = {
    .led_on     = stm32_led_on,
    .led_off    = stm32_led_off,
    .led_toggle = stm32_led_toggle,
    .led_read   = stm32_led_read,
    .btn_read   = stm32_btn_read,
    .uart_send  = stm32_uart_send,
};

/**
 * @brief   Selects which UART instance stm32_uart_send() transmits on.
 * @param   huart: UART handle to bind to the platform's uart_send callback.
 * @retval  None
 */
void Platform_STM32_Init(UART_HandleTypeDef *huart)
{
    s_huart = huart;
}
