/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ILI9341_H
#define __ILI9341_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Exported functions prototypes ---------------------------------------------*/
void ILI9341_Configure(SPI_HandleTypeDef *hspi);
void ILI9341_SendData(SPI_HandleTypeDef *hspi, uint8_t data);
void ILI9341_SendCommand(SPI_HandleTypeDef *hspi, uint8_t data);
void ILI9341_DrawPixel(SPI_HandleTypeDef *hspi, uint16_t x, uint16_t y, uint32_t color);
void ILI9341_SetCursorPosition(SPI_HandleTypeDef *hspi, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/* Private defines -----------------------------------------------------------*/
#define ILI9341_RESET        0x01
#define ILI9341_SLEEP_OUT    0x11
#define ILI9341_DISPLAY_ON   0x29
#define ILI9341_COLUMN_ADDR  0x2A
#define ILI9341_PAGE_ADDR    0x2B
#define ILI9341_MEM_WRITE    0x2C
#define ILI9341_MAC          0x36
#define ILI9341_PIXEL_FORMAT 0x3A
#define ILI9341_FRC          0xB1
#define ILI9341_DFC          0xB6

// To be tested
#define ILI9341_WDB          0x51
#define ILI9341_WCD          0x53

#endif /* __ILI9341_H */
