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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define ILI9341_RESET				0x01
#define ILI9341_SLEEP_OUT			0x11
#define ILI9341_GAMMA				0x26
#define ILI9341_DISPLAY_OFF			0x28
#define ILI9341_DISPLAY_ON			0x29
#define ILI9341_COLUMN_ADDR			0x2A
#define ILI9341_PAGE_ADDR			0x2B
#define ILI9341_GRAM				0x2C
#define ILI9341_MAC					0x36
#define ILI9341_PIXEL_FORMAT		0x3A
#define ILI9341_WDB					0x51
#define ILI9341_WCD					0x53
#define ILI9341_RGB_INTERFACE		0xB0
#define ILI9341_FRC					0xB1
#define ILI9341_BPC					0xB5
#define ILI9341_DFC					0xB6
#define ILI9341_POWER1				0xC0
#define ILI9341_POWER2				0xC1
#define ILI9341_VCOM1				0xC5
#define ILI9341_VCOM2				0xC7
#define ILI9341_POWERA				0xCB
#define ILI9341_POWERB				0xCF
#define ILI9341_PGAMMA				0xE0
#define ILI9341_NGAMMA				0xE1
#define ILI9341_DTCA				0xE8
#define ILI9341_DTCB				0xEA
#define ILI9341_POWER_SEQ			0xED
#define ILI9341_3GAMMA_EN			0xF2
#define ILI9341_INTERFACE			0xF6
#define ILI9341_PRC					0xF7

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

int _write(int fd, char* ptr, int len);

void TM_ILI9341_SendData(uint8_t data);
void TM_ILI9341_SendCommand(uint8_t data);
void TM_ILI9341_Delay(volatile unsigned int delay);
void TM_ILI9341_DrawPixel(uint16_t x, uint16_t y, uint32_t color);
void TM_ILI9341_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

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

  // Set CS high (deselect)
  HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

  // Hardware reset
  HAL_GPIO_WritePin (LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
  for (int i = 0; i < 20000; i++) {}
  HAL_GPIO_WritePin (LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
  for (int i = 0; i < 20000; i++) {}

  //// Software reset
  //uint8_t Data_To_Send[4] = {0};
  //uint8_t Data_Received[4] = {0};
  //Data_To_Send[0] = 0x01;

  //HAL_GPIO_WritePin (LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
  //HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
  //if (HAL_SPI_TransmitReceive(&hspi1, Data_To_Send, Data_Received, 1, HAL_MAX_DELAY) != HAL_OK)
  //{
  //  Error_Handler();
  //}
  //HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

  //for (int i = 0; i < 50000; i++) {}


  //if (HAL_SPI_TransmitReceive(&hspi1, Data_To_Send, Data_Received, 1, HAL_MAX_DELAY) != HAL_OK)
  //{
  //  Error_Handler();
  //}
  //HAL_GPIO_WritePin (LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
  //Data_To_Send[0] = 0;
  //if (HAL_SPI_TransmitReceive(&hspi1, Data_To_Send, Data_Received, 3, HAL_MAX_DELAY) != HAL_OK)
  //{
  //  Error_Handler();
  //}
  //HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

  //HAL_UART_Transmit(&huart1, "man ID: ", 8, HAL_MAX_DELAY);
  //HAL_UART_Transmit(&huart1, &Data_Received[0], 1, HAL_MAX_DELAY);
  //HAL_UART_Transmit(&huart1, "\n", 1, HAL_MAX_DELAY);

  //HAL_UART_Transmit(&huart1, "driver ver: ", 12, HAL_MAX_DELAY);
  //HAL_UART_Transmit(&huart1, &Data_Received[1], 1, HAL_MAX_DELAY);
  //HAL_UART_Transmit(&huart1, "\n", 1, HAL_MAX_DELAY);

  //HAL_UART_Transmit(&huart1, "driver ID: ", 11, HAL_MAX_DELAY);
  //HAL_UART_Transmit(&huart1, &Data_Received[2], 1, HAL_MAX_DELAY);
  //HAL_UART_Transmit(&huart1, "\n", 1, HAL_MAX_DELAY);

	/* Software reset */
	TM_ILI9341_SendCommand(ILI9341_RESET);
	TM_ILI9341_Delay(50000);
	
	TM_ILI9341_SendCommand(ILI9341_POWERA);
	TM_ILI9341_SendData(0x39);
	TM_ILI9341_SendData(0x2C);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x34);
	TM_ILI9341_SendData(0x02);
	TM_ILI9341_SendCommand(ILI9341_POWERB);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0xC1);
	TM_ILI9341_SendData(0x30);
	TM_ILI9341_SendCommand(ILI9341_DTCA);
	TM_ILI9341_SendData(0x85);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x78);
	TM_ILI9341_SendCommand(ILI9341_DTCB);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendCommand(ILI9341_POWER_SEQ);
	TM_ILI9341_SendData(0x64);
	TM_ILI9341_SendData(0x03);
	TM_ILI9341_SendData(0x12);
	TM_ILI9341_SendData(0x81);
	TM_ILI9341_SendCommand(ILI9341_PRC);
	TM_ILI9341_SendData(0x20);
	TM_ILI9341_SendCommand(ILI9341_POWER1);
	TM_ILI9341_SendData(0x23);
	TM_ILI9341_SendCommand(ILI9341_POWER2);
	TM_ILI9341_SendData(0x10);
	TM_ILI9341_SendCommand(ILI9341_VCOM1);
	TM_ILI9341_SendData(0x3E);
	TM_ILI9341_SendData(0x28);
	TM_ILI9341_SendCommand(ILI9341_VCOM2);
	TM_ILI9341_SendData(0x86);
	TM_ILI9341_SendCommand(ILI9341_MAC);
	TM_ILI9341_SendData(0x48);
	TM_ILI9341_SendCommand(ILI9341_PIXEL_FORMAT);
	TM_ILI9341_SendData(0x55);
	TM_ILI9341_SendCommand(ILI9341_FRC);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x18);
	TM_ILI9341_SendCommand(ILI9341_DFC);
	TM_ILI9341_SendData(0x08);
	TM_ILI9341_SendData(0x82);
	TM_ILI9341_SendData(0x27);
	TM_ILI9341_SendCommand(ILI9341_3GAMMA_EN);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendCommand(ILI9341_COLUMN_ADDR);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0xEF);
	TM_ILI9341_SendCommand(ILI9341_PAGE_ADDR);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x01);
	TM_ILI9341_SendData(0x3F);
	TM_ILI9341_SendCommand(ILI9341_GAMMA);
	TM_ILI9341_SendData(0x01);
	TM_ILI9341_SendCommand(ILI9341_PGAMMA);
	TM_ILI9341_SendData(0x0F);
	TM_ILI9341_SendData(0x31);
	TM_ILI9341_SendData(0x2B);
	TM_ILI9341_SendData(0x0C);
	TM_ILI9341_SendData(0x0E);
	TM_ILI9341_SendData(0x08);
	TM_ILI9341_SendData(0x4E);
	TM_ILI9341_SendData(0xF1);
	TM_ILI9341_SendData(0x37);
	TM_ILI9341_SendData(0x07);
	TM_ILI9341_SendData(0x10);
	TM_ILI9341_SendData(0x03);
	TM_ILI9341_SendData(0x0E);
	TM_ILI9341_SendData(0x09);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendCommand(ILI9341_NGAMMA);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x0E);
	TM_ILI9341_SendData(0x14);
	TM_ILI9341_SendData(0x03);
	TM_ILI9341_SendData(0x11);
	TM_ILI9341_SendData(0x07);
	TM_ILI9341_SendData(0x31);
	TM_ILI9341_SendData(0xC1);
	TM_ILI9341_SendData(0x48);
	TM_ILI9341_SendData(0x08);
	TM_ILI9341_SendData(0x0F);
	TM_ILI9341_SendData(0x0C);
	TM_ILI9341_SendData(0x31);
	TM_ILI9341_SendData(0x36);
	TM_ILI9341_SendData(0x0F);
	TM_ILI9341_SendCommand(ILI9341_SLEEP_OUT);

	TM_ILI9341_Delay(1000000);

	TM_ILI9341_SendCommand(ILI9341_DISPLAY_ON);
	TM_ILI9341_SendCommand(ILI9341_GRAM);
	//TM_ILI9341_SendCommand(ILI9341_DISPLAY_OFF);
  //TM_ILI9341_DrawPixel(100, 100, 0x001F);
  //TM_ILI9341_DrawPixel(101, 100, 0x001F);
  //TM_ILI9341_DrawPixel(102, 100, 0x001F);
  //TM_ILI9341_DrawPixel(103, 100, 0x001F);
  //TM_ILI9341_DrawPixel(90, 100, 0x00);
  //TM_ILI9341_DrawPixel(91, 100, 0x00);
  //TM_ILI9341_DrawPixel(92, 100, 0x00);
  //TM_ILI9341_DrawPixel(93, 100, 0x00);
  //for (int x = 0; x < 240; x++)
  //{
  //  for (int y = 0; y < 320; y++)
  //  {
  //    TM_ILI9341_DrawPixel(x, y, 0x001F);
  //  }
  //}

  // CAN configuration

  CAN_FilterTypeDef FilterConfig;
  FilterConfig.FilterIdHigh         = 0xFFFF;
  FilterConfig.FilterIdLow          = 0xFFFF;
  FilterConfig.FilterMaskIdHigh     = 0xFFFF;
  FilterConfig.FilterMaskIdLow      = 0xFFFF;
  FilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  FilterConfig.FilterBank           = 0;
  FilterConfig.FilterMode           = CAN_FILTERMODE_IDMASK;
  FilterConfig.FilterScale          = CAN_FILTERSCALE_32BIT;
  FilterConfig.FilterActivation     = CAN_FILTER_ENABLE;

  if (HAL_CAN_ConfigFilter(&hcan, &FilterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CAN_Start(&hcan) != HAL_OK)
  {
    Error_Handler();
  }

  CAN_RxHeaderTypeDef CAN_Header;
  CAN_TxHeaderTypeDef CAN_Header_TX;
  uint8_t CAN_Payload[8];
  uint8_t CAN_Payload_TX[8];

  uint32_t Mailbox;

  HAL_CAN_StateTypeDef CAN_State;

  CAN_Header_TX.StdId = 0x7DF;
  CAN_Header_TX.IDE = 0;
  CAN_Header_TX.RTR = 0;
  CAN_Header_TX.DLC = 8;
  CAN_Payload_TX[0] = 0x02;
  CAN_Payload_TX[1] = 0x01;
  CAN_Payload_TX[2] = 0x0D;
  CAN_Payload_TX[3] = 0x55;
  CAN_Payload_TX[4] = 0x55;
  CAN_Payload_TX[5] = 0x55;
  CAN_Payload_TX[6] = 0x55;
  CAN_Payload_TX[7] = 0x55;

  printf("Start main loop\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    while (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) > 0)
    {
      HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &CAN_Header, CAN_Payload);
      printf("StdId: %ld, ExtId: %ld, IDE (0 std, 4 ext): %ld, RTR: %ld, DLC: %ld\n", CAN_Header.StdId, CAN_Header.ExtId, CAN_Header.IDE, CAN_Header.RTR, CAN_Header.DLC);
      printf("Payload: %d, %d, %d, %d, %d, %d, %d, %d\n", CAN_Payload[0], CAN_Payload[1], CAN_Payload[2], CAN_Payload[3], CAN_Payload[4], CAN_Payload[5], CAN_Payload[6], CAN_Payload[7]);
    }

    HAL_CAN_AddTxMessage(&hcan, &CAN_Header_TX, CAN_Payload_TX, &Mailbox);

    CAN_State = HAL_CAN_GetState(&hcan);

    printf("State: %d, error: %ld\n", CAN_State, HAL_CAN_GetError(&hcan));


    HAL_GPIO_WritePin (LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    //for (int i = 0; i < 4000000; i++)
    //{
    //}
    //for (int x = 0; x < 240; x++)
    //{
    //  for (int y = 0; y < 320; y++)
    //  {
    //    TM_ILI9341_DrawPixel(x, y, 0x001F);
    //  }
    //}
    HAL_Delay(1000);
    HAL_GPIO_WritePin (LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    //for (int x = 0; x < 240; x++)
    //{
    //  for (int y = 0; y < 320; y++)
    //  {
    //    TM_ILI9341_DrawPixel(x, y, 0xF800);
    //  }
    //}
    HAL_Delay(50);
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

void TM_ILI9341_SendCommand(uint8_t data) {
  uint8_t Data_Received;
  HAL_GPIO_WritePin (LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
  if (HAL_SPI_TransmitReceive(&hspi1, &data, &Data_Received, 1, HAL_MAX_DELAY) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void TM_ILI9341_SendData(uint8_t data) {
  uint8_t Data_Received;
  HAL_GPIO_WritePin (LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
  if (HAL_SPI_TransmitReceive(&hspi1, &data, &Data_Received, 1, HAL_MAX_DELAY) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void TM_ILI9341_Delay(volatile unsigned int delay) {
	for (; delay != 0; delay--); 
}

void TM_ILI9341_DrawPixel(uint16_t x, uint16_t y, uint32_t color) {
	TM_ILI9341_SetCursorPosition(x, y, x, y);

	TM_ILI9341_SendCommand(ILI9341_GRAM);
	TM_ILI9341_SendData(color >> 8);
	TM_ILI9341_SendData(color & 0xFF);
}

void TM_ILI9341_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	TM_ILI9341_SendCommand(ILI9341_COLUMN_ADDR);
	TM_ILI9341_SendData(x1 >> 8);
	TM_ILI9341_SendData(x1 & 0xFF);
	TM_ILI9341_SendData(x2 >> 8);
	TM_ILI9341_SendData(x2 & 0xFF);

	TM_ILI9341_SendCommand(ILI9341_PAGE_ADDR);
	TM_ILI9341_SendData(y1 >> 8);
	TM_ILI9341_SendData(y1 & 0xFF);
	TM_ILI9341_SendData(y2 >> 8);
	TM_ILI9341_SendData(y2 & 0xFF);
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

