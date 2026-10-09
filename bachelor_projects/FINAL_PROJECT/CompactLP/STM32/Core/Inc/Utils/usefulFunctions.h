/**
 ******************************************************************************
 * @file    usefulFunctions.h
 *
 * @brief   Public interface for the UART transmit helper layer: sends a
 *          check: message to the ESP. and the UART_Send*()
 *          family used throughout the project to queue outgoing messages
 *          to the PC/ESP UARTs.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef USEFUL_FUNCTIONS_H
#define USEFUL_FUNCTIONS_H

#include <Utils/common.h>
#include <main.h>

/* Fake functions for voltage/current template  */
uint32_t Sensor_ReadVoltage(void);
uint32_t Sensor_ReadCurrent(void);

/**
 * @brief   Generates a pseudo-random temperature/pressure reading and
 *          sends it to the ESP as a formatted "check" response.
 * @param   huart_esp: destination UART handle.
 * @retval  None
 */
void UTILFN_GenerateData(UART_HandleTypeDef *huart_esp);

/**
 * @brief   Button press handler. Applies a 50 ms software debounce, then sends a
 *          message to the esp: check: t = ...; p = ...; .
 *          - t = [temp_val] => temperature which is sent to the ESP
 *          - p = [pressure_val] => pressure that is send to the esp
 *          - button press can randomly generate values
 *          in given range which are then sent to the ESP.
 *
 *ESP colors this values in:
 *          -red    -> critical color, given value is dangerous
 *          -yellow -> less critical but still concerning
 *          -green  -> value is ok
 *          -blue 	-> cool value, value is quite low
 *
 * @param   huart: destination UART handle passed through to UTILFN_GenerateData().
 * @retval  None
 */
void UTILFN_ButtonPressed(UART_HandleTypeDef *huart);

/**
 * @brief   Sends a raw binary buffer over UART, protected by a mutex shared
 *          with the text-based TX queue so binary and ASCII frames never
 *          interleave on the wire. Unlike UART_Send(), this does NOT use
 *          strnlen() and is safe for buffers containing arbitrary bytes
 *          (including 0x00, \r, \n).
 * @param   huart: destination UART handle.
 * @param   data: raw bytes to send.
 * @param   len: exact number of bytes in data.
 * @retval  None
 */
void UART_SendBinary(UART_HandleTypeDef *huart, const uint8_t *data, uint16_t len);

/**
 * @brief   Stores a msg in the low priority TX queue and notifies the
 *          UART TX task.
 * @param   huart: destination UART handle (or NULL for both).
 * @param   msg: NUL-terminated message to send.
 * @retval  None
 */
void UART_Send(UART_HandleTypeDef *huart, const char *msg);

/**
 * @brief   Stores a msg in the high priority TX queue and notifies the
 *          UART TX task.
 * @param   huart: destination UART handle (or NULL for both).
 * @param   msg: NUL-terminated message to send.
 * @retval  None
 */
void UART_SendHP(UART_HandleTypeDef* huart, const char* msg);

/**
 * @brief   Broadcasts a msg to both PC and ESP on the low priority queue.
 * @param   msg: NUL-terminated message to send.
 * @retval  None
 */
void UART_SendToBoth(const char *msg);

/**
 * @brief   Broadcasts a msg to both PC and ESP on the high priority queue.
 * @param   msg: NUL-terminated message to send.
 * @retval  None
 */
void UART_SendToBothHP(const char *msg);


#endif /* USEFUL_FUNCTIONS_H */
