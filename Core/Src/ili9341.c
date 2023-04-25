/* Includes ------------------------------------------------------------------*/
#include "ili9341.h"
#include "main.h"

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
  ILI9341_SendCommand(hspi, ILI9341_RESET);
  HAL_Delay(50);

  // Configuration
  ILI9341_SendCommand(hspi, ILI9341_MAC);
  ILI9341_SendData(hspi, 0x48);
  ILI9341_SendCommand(hspi, ILI9341_PIXEL_FORMAT);
  ILI9341_SendData(hspi, 0x55);
  ILI9341_SendCommand(hspi, ILI9341_FRC);
  ILI9341_SendData(hspi, 0x00);
  ILI9341_SendData(hspi, 0x18);
  ILI9341_SendCommand(hspi, ILI9341_DFC);
  ILI9341_SendData(hspi, 0x08);
  ILI9341_SendData(hspi, 0x82);
  ILI9341_SendData(hspi, 0x27);
  ILI9341_SendCommand(hspi, ILI9341_COLUMN_ADDR);
  ILI9341_SendData(hspi, 0x00);
  ILI9341_SendData(hspi, 0x00);
  ILI9341_SendData(hspi, 0x00);
  ILI9341_SendData(hspi, 0xEF);
  ILI9341_SendCommand(hspi, ILI9341_PAGE_ADDR);
  ILI9341_SendData(hspi, 0x00);
  ILI9341_SendData(hspi, 0x00);
  ILI9341_SendData(hspi, 0x01);
  ILI9341_SendData(hspi, 0x3F);
  ILI9341_SendCommand(hspi, ILI9341_SLEEP_OUT);
  HAL_Delay(50);
  ILI9341_SendCommand(hspi, ILI9341_DISPLAY_ON);

  for (int x = 0; x < 240; x++)
  {
    for (int y = 0; y < 320; y++)
    {
      ILI9341_DrawPixel(hspi, x, y, 0x001F);
    }
  }
  ILI9341_DrawPixel(hspi, 239, 0, 0xFFFF);
}

void ILI9341_SendCommand(SPI_HandleTypeDef *hspi, uint8_t data) {
  uint8_t Data_Received;
  HAL_GPIO_WritePin (LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  HAL_SPI_TransmitReceive(hspi, &data, &Data_Received, 1, HAL_MAX_DELAY);

  HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void ILI9341_SendData(SPI_HandleTypeDef *hspi, uint8_t data) {
  uint8_t Data_Received;
  HAL_GPIO_WritePin (LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  HAL_SPI_TransmitReceive(hspi, &data, &Data_Received, 1, HAL_MAX_DELAY);

  HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void ILI9341_DrawPixel(SPI_HandleTypeDef *hspi, uint16_t x, uint16_t y, uint32_t color) {
	ILI9341_SetCursorPosition(hspi, x, y, x, y);

	ILI9341_SendCommand(hspi, ILI9341_MEM_WRITE);
	ILI9341_SendData(hspi, color >> 8);
	ILI9341_SendData(hspi, color & 0xFF);
}

void ILI9341_SetCursorPosition(SPI_HandleTypeDef *hspi, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	ILI9341_SendCommand(hspi, ILI9341_COLUMN_ADDR);
	ILI9341_SendData(hspi, x1 >> 8);
	ILI9341_SendData(hspi, x1 & 0xFF);
	ILI9341_SendData(hspi, x2 >> 8);
	ILI9341_SendData(hspi, x2 & 0xFF);

	ILI9341_SendCommand(hspi, ILI9341_PAGE_ADDR);
	ILI9341_SendData(hspi, y1 >> 8);
	ILI9341_SendData(hspi, y1 & 0xFF);
	ILI9341_SendData(hspi, y2 >> 8);
	ILI9341_SendData(hspi, y2 & 0xFF);
}
