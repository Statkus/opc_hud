/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ILI9341_H
#define __ILI9341_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "font.h"

/* Exported functions prototypes ---------------------------------------------*/

// Gride is as follow:
// 0---X
// |
// |
// Y
//
// With X ranging from 0 to ILI9341_WIDTH - 1 pixels and Y ranging from 0 to ILI9341_HEIGHT - 1
// pixels.

void ILI9341_Configure(SPI_HandleTypeDef *hspi);
void ILI9341_Fill_Screen(SPI_HandleTypeDef *hspi, uint16_t color);
void ILI9341_Draw_Pixel(SPI_HandleTypeDef *hspi, uint16_t x, uint16_t y, uint16_t color);
void ILI9341_Draw_Char(SPI_HandleTypeDef *hspi, char c, uint16_t x, uint16_t y, FontTypeDef *font, uint16_t color);
void ILI9341_Draw_String(SPI_HandleTypeDef *hspi, char *str, uint16_t x, uint16_t y, FontTypeDef *font, uint16_t color);
void ILI9341_Draw_Vehicle_Speed(SPI_HandleTypeDef *hspi, uint8_t speed);
void ILI9341_Draw_Water_Temp(SPI_HandleTypeDef *hspi, int16_t temp);
void ILI9341_Draw_Image(SPI_HandleTypeDef *hspi, uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t *data);
void ILI9341_Draw_Boost_Gauge(SPI_HandleTypeDef *hspi, uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t *color_index, uint8_t *data);

/* Private defines -----------------------------------------------------------*/
// ILI9341 registers
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

// ILI9341 constants
#define ILI9341_WIDTH  240
#define ILI9341_HEIGHT 320

// RGB 16 bits colors
#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BLUE      0x001F
#define COLOR_OPC_RED   0xF800
#define COLOR_OPC_BLUE  0x027B

// Vehicle speed display parameters
#define SPEED_X         96
#define SPEED_Y         0
#define SPEED_UNIT_X    149
#define SPEED_UNIT_Y    14
#define SPEED_FONT      Font_16x26
#define SPEED_UNIT_FONT Font_7x10

// Water temperature display parameters
#define TEMP_LOGO_X    0
#define TEMP_LOGO_Y    275
#define TEMP_X         16
#define TEMP_Y         276
#define TEMP_UNIT_X    49
#define TEMP_UNIT_Y    284
#define TEMP_LOGO_FONT Water_Temp_Logo_16x18
#define TEMP_FONT      Font_11x18
#define TEMP_UNIT_FONT Font_7x10

#endif /* __ILI9341_H */
