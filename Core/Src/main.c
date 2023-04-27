/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>

#include "ili9341.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// CAN define
#define CAN_ECU_REQUEST_ID    0x7E0
#define CAN_ECU_REPLY_ID      0x7E8
#define CAN_ECU_DATA_REPLY_ID 0x5E8

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

int16_t Water_Temp    = -41;
int16_t Intake_Temp   = 0;
float Engine_Speed    = 0;
uint8_t Vehicle_Speed = 0;
float MAF             = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

int _write(int fd, char* ptr, int len);
void CAN_RX_Callback(CAN_HandleTypeDef *hcan);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_CAN_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* LCD configuration ****************************************************************************/
  ILI9341_Configure(&hspi1);

  /* CAN configuration ****************************************************************************/
  CAN_FilterTypeDef FilterConfig;
  FilterConfig.FilterIdHigh         = CAN_ECU_REPLY_ID << 5;
  FilterConfig.FilterIdLow          = 0x0000;
  FilterConfig.FilterMaskIdHigh     = 0xFFE0;
  FilterConfig.FilterMaskIdLow      = 0x0000;
  FilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  FilterConfig.FilterBank           = 0;
  FilterConfig.FilterMode           = CAN_FILTERMODE_IDMASK;
  FilterConfig.FilterScale          = CAN_FILTERSCALE_32BIT;
  FilterConfig.FilterActivation     = CAN_FILTER_ENABLE;
  if (HAL_CAN_ConfigFilter(&hcan, &FilterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  FilterConfig.FilterIdHigh         = CAN_ECU_DATA_REPLY_ID << 5;
  FilterConfig.FilterBank           = 1;
  if (HAL_CAN_ConfigFilter(&hcan, &FilterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_CAN_CallbackIDTypeDef Callback_ID = HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID;
  if (HAL_CAN_RegisterCallback(&hcan, Callback_ID, CAN_RX_Callback) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CAN_Start(&hcan) != HAL_OK)
  {
    Error_Handler();
  }

  CAN_TxHeaderTypeDef CAN_Header = {CAN_ECU_REQUEST_ID, 0, 0, 0, 8, DISABLE};
  uint8_t CAN_Water_Temp_Payload[8]    = {0x02, 0x01, 0X05, 0X00, 0X00, 0X00, 0X00, 0X00};
  uint8_t CAN_Engine_Speed_Payload[8]  = {0x02, 0x01, 0X0C, 0X00, 0X00, 0X00, 0X00, 0X00};
  uint8_t CAN_Vehicle_Speed_Payload[8] = {0x02, 0x01, 0X0D, 0X00, 0X00, 0X00, 0X00, 0X00};
  uint8_t CAN_Intake_Temp_Payload[8]   = {0x02, 0x01, 0X0F, 0X00, 0X00, 0X00, 0X00, 0X00};
  uint8_t CAN_MAF_Payload[8]           = {0x02, 0x01, 0X10, 0X00, 0X00, 0X00, 0X00, 0X00};
  uint32_t Mailbox;
  uint8_t Message_Selector = 0;

  int16_t Previous_Water_Temp    = 32767;
  uint8_t Previous_Vehicle_Speed = 255;

  printf("Start main loop\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    HAL_GPIO_WritePin (LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

    HAL_CAN_AddTxMessage(&hcan, &CAN_Header, CAN_Engine_Speed_Payload, &Mailbox);
    HAL_Delay(50);
    HAL_CAN_AddTxMessage(&hcan, &CAN_Header, CAN_MAF_Payload, &Mailbox);
    HAL_Delay(50);

    HAL_GPIO_WritePin (LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

    Message_Selector++;

    switch (Message_Selector)
    {
      case 1:
        HAL_CAN_AddTxMessage(&hcan, &CAN_Header, CAN_Water_Temp_Payload, &Mailbox);
        HAL_Delay(50);
        break;

      case 2:
        HAL_CAN_AddTxMessage(&hcan, &CAN_Header, CAN_Vehicle_Speed_Payload, &Mailbox);
        HAL_Delay(50);
        break;

      case 3:
        HAL_CAN_AddTxMessage(&hcan, &CAN_Header, CAN_Intake_Temp_Payload, &Mailbox);
        HAL_Delay(50);
        Message_Selector = 0;
        break;
    }

    if (Vehicle_Speed != Previous_Vehicle_Speed)
    {
      ILI9341_Draw_Vehicle_Speed(&hspi1, Vehicle_Speed);
      Previous_Vehicle_Speed = Vehicle_Speed;
    }

    if (Water_Temp != Previous_Water_Temp)
    {
      ILI9341_Draw_Water_Temp(&hspi1, Water_Temp);
      Previous_Water_Temp = Water_Temp;
    }

    Vehicle_Speed++;
    Water_Temp++;

    if (Water_Temp > 215)
    {
      Water_Temp = -40;
    }

    //printf("State: %d, error: %ld, Vehicle_Speed: %d, Water_Temp: %d, MAF: %d\n", HAL_CAN_GetState(&hcan), HAL_CAN_GetError(&hcan), Vehicle_Speed, Water_Temp, MAF);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 4;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_3TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_14TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_3TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  huart1.Init.BaudRate = 921600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_RESET_Pin|LCD_DC_Pin|LCD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RESET_Pin LCD_DC_Pin LCD_CS_Pin */
  GPIO_InitStruct.Pin = LCD_RESET_Pin|LCD_DC_Pin|LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

int _write(int fd, char* ptr, int len) {
    HAL_UART_Transmit(&huart1, (uint8_t *) ptr, len, HAL_MAX_DELAY);
    return len;
}

void CAN_RX_Callback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef CAN_Header;
  uint8_t CAN_Payload[8];

  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_Header, CAN_Payload);

  printf("StdId: %ld, DLC: %ld, Payload: %d, %d, %d, %d, %d, %d, %d, %d\n", CAN_Header.StdId, CAN_Header.DLC, CAN_Payload[0], CAN_Payload[1], CAN_Payload[2], CAN_Payload[3], CAN_Payload[4], CAN_Payload[5], CAN_Payload[6], CAN_Payload[7]);

  if (CAN_Header.StdId == CAN_ECU_REPLY_ID)
  {
    switch (CAN_Payload[2])
    {
      case 0x05:
        Water_Temp = (int16_t)(CAN_Payload[3]) - 40;
        break;

      case 0x0C:
        Engine_Speed = (((float)(CAN_Payload[3]) * 256.0) + (float)(CAN_Payload[4])) / 4.0;
        break;

      case 0x0D:
        Vehicle_Speed = CAN_Payload[3];
        break;

      case 0x0F:
        Intake_Temp = (int16_t)(CAN_Payload[3]) - 40;
        break;

      case 0x10:
        MAF = (((float)(CAN_Payload[3]) * 256.0) + (float)(CAN_Payload[4])) / 100.0;
        break;
    }
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();

  printf("An error occured\n");

  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
  printf("Wrong parameters value: file %s on line %d\n", file, line);
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

