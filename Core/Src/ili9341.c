/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include "ili9341.h"
#include "main.h"
#include "image.h"

/* Private function prototypes -----------------------------------------------*/
void ILI9341_Send_Command(SPI_HandleTypeDef *hspi, uint8_t data);
void ILI9341_Send_Data(SPI_HandleTypeDef *hspi, uint8_t data);
void ILI9341_Send_Multiple_Data(SPI_HandleTypeDef *hspi, uint8_t *data, uint16_t size);
void ILI9341_Set_Cursor_Position(SPI_HandleTypeDef *hspi, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ILI9341_Draw_3_Digit_Int(SPI_HandleTypeDef *hspi, int value, uint16_t x, uint16_t y, FontTypeDef *font, uint16_t color);

/* Private user code ---------------------------------------------------------*/
char previous_speed_digit[3] = {' ', ' ', ' '};
char previous_temp_digit[3]  = {' ', ' ', ' '};

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
  ILI9341_Send_Data(hspi, 0x58);
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

  // Fill background in black
  ILI9341_Fill_Screen(hspi, COLOR_BLACK);

  // Draw speed unit
  ILI9341_Draw_String(hspi, "km/h", SPEED_UNIT_X, SPEED_UNIT_Y, &SPEED_UNIT_FONT, COLOR_WHITE);

  // Draw temp unit
  ILI9341_Draw_Char(hspi, ' ', TEMP_LOGO_X, TEMP_LOGO_Y, &TEMP_LOGO_FONT, COLOR_WHITE);
  ILI9341_Draw_Char(hspi, ' ', TEMP_UNIT_X, TEMP_UNIT_Y, &Degree_7x10, COLOR_WHITE);
  ILI9341_Draw_Char(hspi, 'C', TEMP_UNIT_X + 5, TEMP_UNIT_Y, &TEMP_UNIT_FONT, COLOR_WHITE);

  //ILI9341_Draw_Image(hspi, 50, 135, 140, 53, (uint16_t *)opc_logo_140x53_data);
  ILI9341_Draw_Boost_Gauge(hspi, 46, 86, 148, 148, (uint16_t *) opc_boost_gauge_148x148_color_index, (uint8_t *)opc_boost_gauge_148x148_data);
}

void ILI9341_Fill_Screen(SPI_HandleTypeDef *hspi, uint16_t color)
{
  uint8_t data[2] = {color >> 8, color & 0xFF};

  ILI9341_Set_Cursor_Position(hspi, 0, 0, ILI9341_WIDTH - 1, ILI9341_HEIGHT - 1);
  ILI9341_Send_Command(hspi, ILI9341_MEM_WRITE);

  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  for (int i = 0; i < 76800; i++)
  {
    HAL_SPI_Transmit(hspi, data, 2, HAL_MAX_DELAY);
  }

  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void ILI9341_Draw_Pixel(SPI_HandleTypeDef *hspi, uint16_t x, uint16_t y, uint16_t color)
{
  uint8_t data[2] = {color >> 8, color & 0xFF};

  ILI9341_Set_Cursor_Position(hspi, x, y, x, y);

  ILI9341_Send_Command(hspi, ILI9341_MEM_WRITE);
  ILI9341_Send_Multiple_Data(hspi, data, 2);
}

void ILI9341_Draw_Char(SPI_HandleTypeDef *hspi, char c, uint16_t x, uint16_t y, FontTypeDef *font, uint16_t color)
{
  uint16_t b;

  for (uint8_t i = 0; i < font->height; i++) {
    b = font->data[(c - 32) * font->height + i];
    for (uint8_t j = 0; j < font->width; j++) {
      if ((b << j) & 0x8000) {
        ILI9341_Draw_Pixel(hspi, x + j, y + i, color);
      }
    }
  }
}

void ILI9341_Draw_String(SPI_HandleTypeDef *hspi, char *str, uint16_t x, uint16_t y, FontTypeDef *font, uint16_t color)
{
  while (*str)
  {
    ILI9341_Draw_Char(hspi, *str++, x, y, font, color);
    x += font->width;
  }
}

void ILI9341_Draw_Vehicle_Speed(SPI_HandleTypeDef *hspi, uint8_t speed)
{
  char speed_string[4];
  char speed_digit[3];

  sprintf(speed_string, "%d", speed);

  if (speed < 10)
  {
    speed_digit[0] = speed_string[0];
    speed_digit[1] = ' ';
    speed_digit[2] = ' ';
  }
  else if (speed < 100)
  {
    speed_digit[0] = speed_string[1];
    speed_digit[1] = speed_string[0];
    speed_digit[2] = ' ';
  }
  else
  {
    speed_digit[0] = speed_string[2];
    speed_digit[1] = speed_string[1];
    speed_digit[2] = speed_string[0];
  }

  if (speed_digit[0] != previous_speed_digit[0])
  {
    ILI9341_Draw_Char(hspi, previous_speed_digit[0], SPEED_X + 2 * SPEED_FONT.width, SPEED_Y, &SPEED_FONT, COLOR_BLACK);
    ILI9341_Draw_Char(hspi, speed_digit[0], SPEED_X + 2 * SPEED_FONT.width, SPEED_Y, &SPEED_FONT, COLOR_WHITE);
    previous_speed_digit[0] = speed_digit[0];
  }

  if (speed_digit[1] != previous_speed_digit[1])
  {
    ILI9341_Draw_Char(hspi, previous_speed_digit[1], SPEED_X + SPEED_FONT.width, SPEED_Y, &SPEED_FONT, COLOR_BLACK);
    ILI9341_Draw_Char(hspi, speed_digit[1], SPEED_X + SPEED_FONT.width, SPEED_Y, &SPEED_FONT, COLOR_WHITE);
    previous_speed_digit[1] = speed_digit[1];
  }

  if (speed_digit[2] != previous_speed_digit[2])
  {
    ILI9341_Draw_Char(hspi, previous_speed_digit[2], SPEED_X, SPEED_Y, &SPEED_FONT, COLOR_BLACK);
    ILI9341_Draw_Char(hspi, speed_digit[2], SPEED_X, SPEED_Y, &SPEED_FONT, COLOR_WHITE);
    previous_speed_digit[2] = speed_digit[2];
  }
}

void ILI9341_Draw_Water_Temp(SPI_HandleTypeDef *hspi, int16_t temp)
{
  char temp_string[7];
  char temp_digit[3];

  sprintf(temp_string, "%d", temp);

  if (temp < -9)
  {
    temp_digit[0] = temp_string[2];
    temp_digit[1] = temp_string[1];
    temp_digit[2] = temp_string[0];
  }
  else if (temp < 0)
  {
    temp_digit[0] = temp_string[1];
    temp_digit[1] = temp_string[0];
    temp_digit[2] = ' ';
  }
  else if (temp < 10)
  {
    temp_digit[0] = temp_string[0];
    temp_digit[1] = ' ';
    temp_digit[2] = ' ';
  }
  else if (temp < 100)
  {
    temp_digit[0] = temp_string[1];
    temp_digit[1] = temp_string[0];
    temp_digit[2] = ' ';
  }
  else
  {
    temp_digit[0] = temp_string[2];
    temp_digit[1] = temp_string[1];
    temp_digit[2] = temp_string[0];
  }

  if (temp_digit[0] != previous_temp_digit[0])
  {
    ILI9341_Draw_Char(hspi, previous_temp_digit[0], TEMP_X + 2 * TEMP_FONT.width, TEMP_Y, &TEMP_FONT, COLOR_BLACK);
    ILI9341_Draw_Char(hspi, temp_digit[0], TEMP_X + 2 * TEMP_FONT.width, TEMP_Y, &TEMP_FONT, COLOR_WHITE);
    previous_temp_digit[0] = temp_digit[0];
  }

  if (temp_digit[1] != previous_temp_digit[1])
  {
    ILI9341_Draw_Char(hspi, previous_temp_digit[1], TEMP_X + TEMP_FONT.width, TEMP_Y, &TEMP_FONT, COLOR_BLACK);
    ILI9341_Draw_Char(hspi, temp_digit[1], TEMP_X + TEMP_FONT.width, TEMP_Y, &TEMP_FONT, COLOR_WHITE);
    previous_temp_digit[1] = temp_digit[1];
  }

  if (temp_digit[2] != previous_temp_digit[2])
  {
    ILI9341_Draw_Char(hspi, previous_temp_digit[2], TEMP_X, TEMP_Y, &TEMP_FONT, COLOR_BLACK);
    ILI9341_Draw_Char(hspi, temp_digit[2], TEMP_X, TEMP_Y, &TEMP_FONT, COLOR_WHITE);
    previous_temp_digit[2] = temp_digit[2];
  }
}

void ILI9341_Draw_Image(SPI_HandleTypeDef *hspi, uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t *data)
{

  ILI9341_Set_Cursor_Position(hspi, x, y, x + size_x - 1, y + size_y - 1);
  ILI9341_Send_Command(hspi, ILI9341_MEM_WRITE);

  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  for (int i = 0; i < size_x * size_y; i++)
  {
    uint8_t data_color[2] = {data[i] >> 8, data[i] & 0xFF};
    HAL_SPI_Transmit(hspi, data_color, 2, HAL_MAX_DELAY);
  }

  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void ILI9341_Draw_Boost_Gauge(SPI_HandleTypeDef *hspi, uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t *color_index, uint8_t *data)
{
  ILI9341_Set_Cursor_Position(hspi, x, y, x + size_x - 1, y + size_y - 1);
  ILI9341_Send_Command(hspi, ILI9341_MEM_WRITE);

  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  for (int i = 0; i < size_x * size_y; i++)
  {
    uint8_t data_color[2] = {color_index[data[i]] >> 8, color_index[data[i]] & 0xFF};

    HAL_SPI_Transmit(hspi, data_color, 2, HAL_MAX_DELAY);
  }

  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
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

void ILI9341_Draw_3_Digit_Int(SPI_HandleTypeDef *hspi, int value, uint16_t x, uint16_t y, FontTypeDef *font, uint16_t color)
{
  char value_string[4];

  sprintf(value_string, "%d", value);

  switch (strlen(value_string))
  {
    case 1:
      ILI9341_Draw_String(hspi, value_string, x + 2 * font->width, y, font, color);
      break;

    case 2:
      ILI9341_Draw_String(hspi, value_string, x + font->width, y, font, color);
      break;

    default:
      ILI9341_Draw_String(hspi, value_string, x, y, font, color);
      break;
  }
}
