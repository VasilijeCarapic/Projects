/**
 ******************************************************************************
 * @file    common.h
 *
 * @brief   This file will be used as global constants / types that are used in multiple files
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    September, 2026
 ******************************************************************************
 */
#ifndef _COMMON_H
#define _COMMON_H

#include "FreeRTOS.h"

#define SYNQ_BYTES        ( 4 ) 	/*sync+sid+len */
#define MAGIC_CTR_BYTES   ( 8 )     /*counter+magic */
#define SAMPLE_SIZE       ( 250 )   /* samples num */
#define CURRENT_BYTES     ( 2 )     /* current 2 bytes width */
#define VOLTAGE_BYTES     ( 2 )		/* voltage 2 bytes width */
#define FRAME_LEN         ( SYNQ_BYTES + MAGIC_CTR_BYTES + SAMPLE_SIZE * CURRENT_BYTES * VOLTAGE_BYTES)

#define SENSOR_TASK_DELAY pdMS_TO_TICKS(15)

typedef enum {
	DEVICE_ACQUISITION_INT,
	DEVICE_ACQUISITION_EXT,
	DEVICE_ACQUISTION_UNDEF
} device_acquisition_type_t;

extern device_acquisition_type_t SENSOR_TASK_ACQUISITION_TYPE;

/* Default OK and Error responses*/
// #define CONTROL_RESPONSE_ERROR_STATUS_MSG "ERROR"
// #define CONTROL_RESPONSE_OK_STATUS_MSG    "OK"

/**/
// #define MAX_LEN_DEBUG_MSG                 128
// #define MAX_LEN_RESP_MSG                  96

#endif
