/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ILI9341_H
#define __ILI9341_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Exported functions prototypes ---------------------------------------------*/

// Gride is as follow:
// Y-------0
//         |
//         |
//         X
//
// with X = 240 pixels and Y = 320 pixels

void ILI9341_Configure(SPI_HandleTypeDef *hspi);
void ILI9341_Fill_Color(SPI_HandleTypeDef *hspi, uint16_t color);
void ILI9341_Draw_Pixel(SPI_HandleTypeDef *hspi, uint16_t x, uint16_t y, uint16_t color);

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

#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_RED   0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE  0x001F

#endif /* __ILI9341_H */
