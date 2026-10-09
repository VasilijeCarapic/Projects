/**
 ******************************************************************************
 * @file    txRouter.h
 *
 * @brief   Structures which make it easier for the data to be sent to the UART Tx Task.
 * 			UART Tx task then sends that data to the ESP or to the PC.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef TX_ROUTER_H
#define TX_ROUTER_H

#include "stm32l4xx_hal.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#define TX_QUEUE_SIZE 10

/**
 * @brief   Routing destination for a queued TX message.
 */
typedef enum {
    TX_DEST_PC,   /*!< Send only to the PC UART. */
    TX_DEST_ESP,  /*!< Send only to the ESP UART. */
    TX_DEST_BOTH  /*!< Send to both UARTs. */
} TxDest_t;

/**
 * @brief   A single message queued for transmission by the UART TX task.
 */
typedef struct {
    uint8_t  data[150]; /*!< NUL-terminated message payload. */
    uint16_t len;       /*!< Payload length in bytes (excluding the NUL). */
    TxDest_t dest;      /*!< Resolved destination(s) for this message. */
} TxMsg_t;

extern QueueHandle_t qTxHigh;   /* SEN task -> UART TX task */
extern QueueHandle_t qTxLow;    /* PARSER task -> UART TX task */

#endif
