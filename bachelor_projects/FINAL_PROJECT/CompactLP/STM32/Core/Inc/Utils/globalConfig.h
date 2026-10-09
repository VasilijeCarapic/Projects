/**
 ******************************************************************************
 * @file   	globalConfig.h
 *
 * @brief  	All Global configuration macros are declared in this header file.
 *
 * @author	Haris Turkmanovic
 * @email	haris.turkmanovic@gmail.com
 * @date	November 2023
 ******************************************************************************
 */

#ifndef CORE_CONFIGURATION_GLOBALCONFIG_H_
#define CORE_CONFIGURATION_GLOBALCONFIG_H_


/**
 * @defgroup CONFIGURATION System Configuration
 * @{
 */

/**
 * @defgroup GLOBALCONFIG_CONFIG Global System Configuration
 * @{
 */

/**
 * @defgroup GLOBALCONFIG_PUBLIC_DEFINES Global System Configuration public defines
 * @{
 */

/* Driver layer configuration */
/* Timer configuration */
#define CONF_DRV_TIMER_MAX_NUMBER_OF_TIMERS 		3
#define CONF_DRV_TIMER_MAX_NUMBER_OF_CHANNELS 		4

/* GPIO configuration */
#define CONF_GPIO_PORT_MAX_NUMBER					8
#define CONF_GPIO_PIN_MAX_NUMBER					16
#define	CONF_GPIO_INTERRUPTS_MAX_NUMBER				15

#define	CONF_UART_INSTANCES_MAX_NUMBER				5
#define	CONF_I2C_INSTANCES_MAX_NUMBER				5

/* AnalogIN configuration (L476: 12-bit ADC, 80 MHz APB clock) */
#define CONF_AIN_MAX_BUFFER_SIZE					100
#define CONF_AIN_ADC_BUFFER_OFFSET					2
#define CONF_AIN_ADC_BUFFER_MARKER					0xA5A5
#define CONF_AIN_MAX_BUFFER_NO						2
#define CONF_DRV_AIN_ADC_TIM_INPUT_CLK				80000000  /* Hz - APB2 @ 80 MHz */

/* AnalogOUT configuration */
#define CONF_AOUT_MAX_CHANNELS						2

/* Middleware layer configuration */


/* Status LED: LD2 on NUCLEO-L476RG is PA5 */
#define	CONF_SYSTEM_ERROR_STATUS_DIODE_PORT			0	/* Port A */
#define	CONF_SYSTEM_ERROR_STATUS_DIODE_PIN			5

#define	CONF_SYSTEM_LINK_STATUS_DIODE_PORT			0	/* Port A */
#define	CONF_SYSTEM_LINK_STATUS_DIODE_PIN			5

/* Charger service */
#define CONF_CURRENT_SENSE_TASK_NAME				"Current Task"
#define	CONF_CURRENT_SENSE_STACK_SIZE				1024
#define	CONF_CURRENT_SENSE_PRIO						3
#define CONF_CURRENT_SENSE_POLLING_PERIOD           1000


/**
 * @defgroup GLOBALCONFIG_CONFIGURATION_CONFIG Configuration service configuration
 * @{
 */

#define CONF_CONFIGURATION_ENABLE                   1       /*!< Enable/disable configuration service */

#define CONF_CONFIGURATION_TASK_NAME                "Configuration service" /*!< Configuration task name */
#define CONF_CONFIGURATION_TASK_PRIO                3       /*!< Configuration task priority */
#define CONF_CONFIGURATION_TASK_STACK_SIZE          1024    /*!< Configuration task stack size */

#define CONF_CONFIGURATION_MAX_PARAMS               30      /*!< Maximum number of configuration parameters */
#define CONF_CONFIGURATION_MAX_PARAM_VALUESIZE      32      /*!< Maximum configuration parameter value length */

#define CONF_CONFIGURATION_HEADER_SIZE              8       /*!< Configuration file header size */






/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */
#endif /* CORE_CONFIGURATION_GLOBALCONFIG_H_ */
