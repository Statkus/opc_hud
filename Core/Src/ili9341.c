/* Includes ------------------------------------------------------------------*/
#include "ili9341.h"
#include "main.h"

/* Private function prototypes -----------------------------------------------*/
void ILI9341_Send_Command(SPI_HandleTypeDef *hspi, uint8_t data);
void ILI9341_Send_Data(SPI_HandleTypeDef *hspi, uint8_t data);
void ILI9341_Send_Multiple_Data(SPI_HandleTypeDef *hspi, uint8_t *data, uint16_t size);
void ILI9341_Set_Cursor_Position(SPI_HandleTypeDef *hspi, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/* Private user code ---------------------------------------------------------*/
void ILI9341_Configure(SPI_HandleTypeDef *hspi)
{
  // Set CS high (deselect)
  HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

  // Hardware reset
  HAL_GPIO_WritePin (LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(50);
  HAL_GPIO_WritePin (LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
  HAL_Delay(50);

  // Software reset
  ILI9341_Send_Command(hspi, ILI9341_RESET);
  HAL_Delay(50);

  // Configuration
  ILI9341_Send_Command(hspi, ILI9341_MAC);
  ILI9341_Send_Data(hspi, 0x48);
  ILI9341_Send_Command(hspi, ILI9341_PIXEL_FORMAT);
  ILI9341_Send_Data(hspi, 0x55);
  ILI9341_Send_Command(hspi, ILI9341_FRC);
  ILI9341_Send_Data(hspi, 0x00);
  ILI9341_Send_Data(hspi, 0x18);
  ILI9341_Send_Command(hspi, ILI9341_DFC);
  ILI9341_Send_Data(hspi, 0x08);
  ILI9341_Send_Data(hspi, 0x82);
  ILI9341_Send_Data(hspi, 0x27);
  ILI9341_Send_Command(hspi, ILI9341_COLUMN_ADDR);
  ILI9341_Send_Data(hspi, 0x00);
  ILI9341_Send_Data(hspi, 0x00);
  ILI9341_Send_Data(hspi, 0x00);
  ILI9341_Send_Data(hspi, 0xEF);
  ILI9341_Send_Command(hspi, ILI9341_PAGE_ADDR);
  ILI9341_Send_Data(hspi, 0x00);
  ILI9341_Send_Data(hspi, 0x00);
  ILI9341_Send_Data(hspi, 0x01);
  ILI9341_Send_Data(hspi, 0x3F);
  ILI9341_Send_Command(hspi, ILI9341_SLEEP_OUT);
  HAL_Delay(50);
  ILI9341_Send_Command(hspi, ILI9341_DISPLAY_ON);

  ILI9341_Fill_Color(hspi, COLOR_BLACK);
}

void ILI9341_Fill_Color(SPI_HandleTypeDef *hspi, uint16_t color)
{
  ILI9341_Set_Cursor_Position(hspi, 0, 0, 239, 319);
  ILI9341_Send_Command(hspi, ILI9341_MEM_WRITE);

  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  uint8_t data[2] = {color >> 8, color & 0xFF};

  for (int i = 0; i < 76800; i++)
  {
    HAL_SPI_Transmit(hspi, data, 2, HAL_MAX_DELAY);
  }

  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void ILI9341_Draw_Pixel(SPI_HandleTypeDef *hspi, uint16_t x, uint16_t y, uint16_t color)
{
  ILI9341_Set_Cursor_Position(hspi, x, y, x, y);

  ILI9341_Send_Command(hspi, ILI9341_MEM_WRITE);
  ILI9341_Send_Data(hspi, color >> 8);
  ILI9341_Send_Data(hspi, color & 0xFF);
}

void ILI9341_Send_Command(SPI_HandleTypeDef *hspi, uint8_t data)
{
  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  HAL_SPI_Transmit(hspi, &data, 1, HAL_MAX_DELAY);

  HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void ILI9341_Send_Data(SPI_HandleTypeDef *hspi, uint8_t data)
{
  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  HAL_SPI_Transmit(hspi, &data, 1, HAL_MAX_DELAY);

  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void ILI9341_Send_Multiple_Data(SPI_HandleTypeDef *hspi, uint8_t *data, uint16_t size)
{
  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  HAL_SPI_Transmit(hspi, data, size, HAL_MAX_DELAY);

  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void ILI9341_Set_Cursor_Position(SPI_HandleTypeDef *hspi, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  uint8_t column_data[4] = {x1 >> 8, x1 & 0xFF, x2 >> 8, x2 & 0xFF};
  uint8_t page_data[4]   = {y1 >> 8, y1 & 0xFF, y2 >> 8, y2 & 0xFF};

  ILI9341_Send_Command(hspi, ILI9341_COLUMN_ADDR);
  ILI9341_Send_Multiple_Data(hspi, column_data, 4);

  ILI9341_Send_Command(hspi, ILI9341_PAGE_ADDR);
  ILI9341_Send_Multiple_Data(hspi, page_data, 4);
}
