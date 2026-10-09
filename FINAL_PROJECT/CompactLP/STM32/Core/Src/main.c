/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c3;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_UART4_Init(void);
static void MX_I2C3_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

QueueHandle_t qTxHigh;
QueueHandle_t qTxLow;
SemaphoreHandle_t uartTxGuard;

/* set to 1 in USER CODE 2 if INA234_Init() succeeds; Sensor_ReadVoltage()/
 * Sensor_ReadCurrent() (usefulFunctions.c) fall back to the old dummy
 * values whenever this is 0 (sensor not found / not wired yet). */
volatile uint8_t ina234Ready = 0;

// for PC UART
volatile uint8_t rx_byte;
volatile char rx_message[100];
volatile uint8_t rx_index = 0;
volatile uint8_t command_ready = 0;
char command_buf[100];

// for ESP UART
volatile uint8_t rx_byte_esp;
volatile char rx_message_esp[100];
volatile uint8_t rx_index_esp = 0;
volatile uint8_t command_ready_esp = 0;
char command_buf_esp[100];

/**
 * @brief   Test function for I2C setup and configuration. Pings every
 *          7-bit I2C address (1-127) on the I2C1 bus with
 *          HAL_I2C_IsDeviceReady() and reports each responding address, as
 *          well as the total count, over huart2. If there is no address reports no addresses.
 * @retval  None
 */
void I2C_Scan(void)
{
    char info[64];
    UART_Send(&huart2, "Pokrecem I2C Skener...\r\n");

    uint8_t count = 0;

    // I2C addresses are in range from 1 to 127
    for (uint16_t i = 1; i < 128; i++)
    {
        // Trying to ping the device on an address of i
    	// Shift address for 1 lefr cause HAL library accepts 8-bit format
        if (HAL_I2C_IsDeviceReady(&hi2c3, (uint16_t)(i << 1), 3, 5) == HAL_OK)
        {
            snprintf(info, sizeof(info), "Pronadjen I2C uredjaj na adresi: 0x%02X\r\n", i);
            UART_Send(&huart2, info);
            count++;
        }
    }

    if (count == 0)
    {
        UART_Send(&huart2, "Nije pronadjen nijedan I2C uredjaj.\r\n");
    }
    UART_Send(&huart2, "Skeniranje zavrseno.\r\n");
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_UART4_Init();
  MX_I2C3_Init();

  {
    /* TODO: shuntResistance/currentLSB are hardware-specific - confirm the
     * actual shunt value with Haris (or measure it) before trusting the
     * current/power readings. Bus voltage does not depend on these two.
     * Shunt value (40 mOhm / R5) taken from compact_low_power schematic. */
    ina234_config_t ina234Cfg;
    ina234Cfg.range          = INA234_ADC_RANGE_81_92_MV;
    ina234Cfg.averaging      = INA234_AVG_16;
    ina234Cfg.busConvTime    = INA234_CONV_TIME_1100_US;
    ina234Cfg.shuntConvTime  = INA234_CONV_TIME_1100_US;
    ina234Cfg.mode           = INA234_MODE_CONTINUOUS_SHUNT_BUS;
    ina234Cfg.shuntResistance = 0.040f;
    ina234Cfg.currentLSB      = 0.001f;

    /* NOTE: UART_Send() can't be used here yet - it needs qTxLow/UartTxTaskHandle,
     * which don't exist until the "Create FreeRtos Objects" block below runs.
     * Scheduler hasn't started either, so a plain blocking transmit is safe. */
    if (INA234_Init(&ina234Cfg, 1000U) == INA234_STATUS_OK)
    {
      ina234Ready = 1;
      HAL_UART_Transmit(&huart2, (uint8_t*)"INA234 successfully initialized.\r\n", 35, 100);
    }
    else
    {
      ina234Ready = 0;
      HAL_UART_Transmit(&huart2, (uint8_t*)"INA234 not found - falling back to dummy sensor values.\r\n", 58, 100);
    }
  }

  STREAMCFG_ConfigurationInit();
  Platform_STM32_Init(&huart2);

  /* Create FreeRtos Objects */
  uartTxGuard = xSemaphoreCreateMutex();
  qTxHigh = xQueueCreate(TX_QUEUE_SIZE, sizeof(TxMsg_t));
  qTxLow = xQueueCreate(TX_QUEUE_SIZE, sizeof(TxMsg_t));

  /* Create the thread(s) */
  xTaskCreate(prvParserTaskFunction,
			"Parser Task",
			PARSER_TASK_STACK_SIZE,
			NULL,
			PARSER_TASK_PRIO,
			&ParserTaskHandle);

  xTaskCreate(prvSensorTaskFunction,
  			"Sensor Task",
  			SENSOR_TASK_STACK_SIZE,
  			NULL,
  			SENSOR_TASK_PRIO,
  			&SensorTaskHandle);

  xTaskCreate(prvUartTxTaskFunction,
  			"UART Tx Task",
  			UART_TX_TASK_STACK_SIZE,
  			NULL,
			UART_TX_TASK_PRIO,
  			&UartTxTaskHandle);

  /* Start scheduler */
  vTaskStartScheduler();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x10D19CE4;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 921600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
 * @brief   HAL UART receive-complete callback, shared by both UARTs.
 *
 *          Called once per received byte. Accumulates
 *          bytes into rx_message[] (PC / USART2) or rx_message_esp[] (ESP
 *          / UART4) until a '\r' or '\n' terminator is seen; on
 *          terminator, if a line was accumulated and no command is
 *          already pending, copies it into command_buf[] /
 *          command_buf_esp[], sets command_ready / command_ready_esp, and
 *          notifies ParserTaskHandle with MSG_RECEIVED_FLAG from ISR
 *          context. Always re-arms HAL_UART_Receive_IT() for the next
 *          byte and yields to a higher-priority task if one was woken.
 *
 * @param   huart: UART handle that completed reception (huart4 or
 *          huart2).
 * @retval  None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (huart->Instance == USART2)
    {
        if (rx_byte != '\r' && rx_byte != '\n')
        {
            rx_message[rx_index++] = rx_byte;
            if (rx_index >= sizeof(rx_message) - 1) rx_index = 0;
        }
        else
        {
            rx_message[rx_index] = '\0';
            if (rx_index > 0 && !command_ready)
            {
                strncpy(command_buf, (const char*)rx_message, sizeof(command_buf) - 1);
                command_buf[sizeof(command_buf) - 1] = '\0';
                command_ready = 1;
                xTaskNotifyFromISR(ParserTaskHandle, MSG_RECEIVED_FLAG, eSetBits, &xHigherPriorityTaskWoken);
            }
            rx_index = 0;
        }
        HAL_UART_Receive_IT(&huart2, (uint8_t*)&rx_byte, 1);
    }
    else if (huart->Instance == UART4)
    {
        if (rx_byte_esp != '\r' && rx_byte_esp != '\n')
        {
            rx_message_esp[rx_index_esp++] = rx_byte_esp;
            if (rx_index_esp >= sizeof(rx_message_esp) - 1) rx_index_esp = 0;
        }
        else
        {
            rx_message_esp[rx_index_esp] = '\0';
            if (rx_index_esp > 0 && !command_ready_esp)
            {
                strncpy(command_buf_esp, (const char*)rx_message_esp, sizeof(command_buf_esp) - 1);
                command_buf_esp[sizeof(command_buf_esp) - 1] = '\0';
                command_ready_esp = 1;
                xTaskNotifyFromISR(ParserTaskHandle, MSG_RECEIVED_FLAG, eSetBits, &xHigherPriorityTaskWoken);
            }
            rx_index_esp = 0;
        }
        HAL_UART_Receive_IT(&huart4, (uint8_t*)&rx_byte_esp, 1);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_13)
    {
    	UTILFN_ButtonPressed(&huart4);
    }
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
