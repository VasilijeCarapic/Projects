/**
 ******************************************************************************
 * @file    cmdResponse.h
 *
 * @brief   Response-formatting helpers for the ASCII command protocol: builds
 *          OK/Error response strings and sends them over UART.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    September, 2026
 ******************************************************************************
 */

#ifndef CMD_RESPONSE_H
#define CMD_RESPONSE_H

#include <stdint.h>
#include <string.h>
#include <Platform/platformStm32.h>
#include "stm32l4xx_hal.h"

/* Default OK and Error responses */
#define CONTROL_RESPONSE_ERROR_STATUS_MSG "ERROR"
#define CONTROL_RESPONSE_OK_STATUS_MSG    "OK"

/*mMaximum allowed msg lengths */
#define MAX_LEN_DEBUG_MSG                 128
#define MAX_LEN_RESP_MSG                  96

/**
 * @brief prepare OK response
 * @param resp	  : response
 * @param msg 	  : OK + this parameter is full resp
 * @param respSize: size of the response
 * @param msgSize : size of the message
 */
void prvCONTROL_PrepareOkResp(char* resp, char* msg, uint32_t respSize, uint32_t msgSize);

/**
 * @brief prepare Error response
 * @param resp	  : response
 * @param respSize: size of the response
 */
void prvCONTROL_PrepareErrorResp(char* resp, uint32_t respSize);

/**
 * @brief sendOKResponse via Uart
 * @param resp	  : response
 * @param respSize: size of the response
 */
void prvCONTROL_SendOkResp(UART_HandleTypeDef *huart_esp, char* response,
		uint32_t respSize, const char* msg, uint32_t msgSize);


/**
 * @brief SendOKResponse to High priority Queue
 *
 */
void prvCONTROL_SendOkRespHP(UART_HandleTypeDef *huart_esp, char* response, uint32_t respSize,
		const char* msg, uint32_t msgSize);


/**
 * @brief Send Error Response via Uart
 */
void prvCONTROL_SendErrorResp(UART_HandleTypeDef *huart_esp, char* response, uint32_t respSize);

#endif /* CMD_RESPONSE_H */
