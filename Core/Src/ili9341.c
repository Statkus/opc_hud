/* Includes ------------------------------------------------------------------*/
#include <math.h>
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
void ILI9341_Draw_Line(SPI_HandleTypeDef *hspi, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void ILI9341_Draw_Line_With_Data_Color(SPI_HandleTypeDef *hspi, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t offset_x, uint16_t offset_y, uint16_t size_x, uint16_t *color_index, uint8_t *data);

static inline uint32_t Fetch_Bit(const uint8_t *p, uint32_t index);
static uint32_t Fetch_Bits_Unsigned(const uint8_t *p, uint32_t index, uint32_t required);
static uint32_t Fetch_Bits_Signed(const uint8_t *p, uint32_t index, uint32_t required);
void New_Draw_Char(SPI_HandleTypeDef *hspi, char c, const ILI9341_t3_font_t *font, uint16_t color);
void New_Draw_String(SPI_HandleTypeDef *hspi, char *str, const ILI9341_t3_font_t *font, uint16_t color);
void Draw_Font_Bits(SPI_HandleTypeDef *hspi, uint32_t bits, uint16_t num_bits, uint16_t x, uint16_t y, uint8_t repeat, uint16_t color);
uint16_t Get_Char_Width(char c, const ILI9341_t3_font_t *font);

/* Private user code ---------------------------------------------------------*/
char previous_speed_digit[3]  = {' ', ' ', ' '};
char previous_temp_digit[3]   = {' ', ' ', ' '};
char previous_itemp_digit[3]  = {' ', ' ', ' '};
char previous_MAF_digit[3]    = {' ', ' ', ' '};
char previous_boost_digit[5]  = {' ', ' ', ' ', ' ', ' '};
uint16_t previous_x0 = 74;
uint16_t previous_y0 = 74;
uint16_t previous_x1 = 74;
uint16_t previous_y1 = 74;

uint32_t gauge_pointer_pixel[512] = {0};
uint32_t previous_gauge_pointer_pixel[512] = {0};

int16_t cursor_x = 0;
int16_t cursor_y = 0;

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

  // Draw vehicle speed unit
  cursor_x = SPEED_UNIT_X;
  cursor_y = SPEED_UNIT_Y;
  New_Draw_String(hspi, "km/h", &SPEED_UNIT_FONT, COLOR_WHITE);

  // Draw water temp unit
  ILI9341_Draw_Char(hspi, ' ', TEMP_LOGO_X, TEMP_LOGO_Y, &TEMP_LOGO_FONT, COLOR_WHITE);
  ILI9341_Draw_Char(hspi, ' ', TEMP_UNIT_X, TEMP_UNIT_Y, &Degree_7x10, COLOR_WHITE);
  cursor_x = TEMP_UNIT_X + 5;
  cursor_y = TEMP_UNIT_Y;
  New_Draw_Char(hspi, 'C', &TEMP_UNIT_FONT, COLOR_WHITE);

  // Draw intake temp unit
  cursor_x = ITEMP_LOGO_X;
  cursor_y = ITEMP_LOGO_Y;
  New_Draw_String(hspi, "IAT:", &ITEMP_LOGO_FONT, COLOR_WHITE);
  ILI9341_Draw_Char(hspi, ' ', ITEMP_UNIT_X, ITEMP_UNIT_Y, &Degree_7x10, COLOR_WHITE);
  cursor_x = ITEMP_UNIT_X + 5;
  cursor_y = ITEMP_UNIT_Y;
  New_Draw_Char(hspi, 'C', &ITEMP_UNIT_FONT, COLOR_WHITE);

  // Draw boost gauge
  ILI9341_Draw_Boost_Gauge(hspi, GAUGE_X, GAUGE_Y, GAUGE_WIDTH, GAUGE_HEIGHT, (uint16_t *) opc_boost_gauge_148x148_color_index, (uint8_t *)opc_boost_gauge_148x148_data);
  ILI9341_Draw_Boost_Gauge_Pointer(hspi, GAUGE_X, GAUGE_Y, GAUGE_WIDTH, GAUGE_HEIGHT, (uint16_t *) opc_boost_gauge_148x148_color_index, (uint8_t *)opc_boost_gauge_148x148_data, 0);

  // Draw boost unit
  cursor_x = BOOST_UNIT_X;
  cursor_y = BOOST_UNIT_Y;
  New_Draw_String(hspi, "bar", &BOOST_UNIT_FONT, COLOR_WHITE);

  // Draw boost value dot
  cursor_x = BOOST_X + 2 * BOOST_FONT_WIDTH;
  cursor_y = BOOST_Y;
  New_Draw_Char(hspi, '.', &BOOST_FONT, COLOR_WHITE);
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

void ILI9341_Fill_Rectangle(SPI_HandleTypeDef *hspi, uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t color)
{
  uint8_t data[2] = {color >> 8, color & 0xFF};

  ILI9341_Set_Cursor_Position(hspi, x, y, x + size_x - 1, y + size_y - 1);
  ILI9341_Send_Command(hspi, ILI9341_MEM_WRITE);

  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  const uint32_t size = size_x * size_y;

  for (int i = 0; i < size; i++)
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

void ILI9341_Draw_Large_Pixel(SPI_HandleTypeDef *hspi, uint16_t x, uint16_t y, uint16_t color)
{
  uint8_t data[50] = {
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF,
    color >> 8, color & 0xFF};

  ILI9341_Set_Cursor_Position(hspi, x - 2, y - 2, x + 2, y + 2);

  ILI9341_Send_Command(hspi, ILI9341_MEM_WRITE);
  ILI9341_Send_Multiple_Data(hspi, data, 50);

  const int k = x - 2 - GAUGE_POINTER_X + ((y - 2 - GAUGE_POINTER_Y) * GAUGE_POINTER_WIDTH);
  for (int i = 0; i < 5; i++)
  {
    for (int j = 0; j < 5; j++)
    {
      int pixel = k + i + (j * GAUGE_POINTER_WIDTH);
      gauge_pointer_pixel[pixel / 32] |= 1 << (pixel % 32);
    }
  }
}

void ILI9341_Draw_Large_Pixel_With_Data_Color(SPI_HandleTypeDef *hspi, uint16_t x, uint16_t y, uint16_t offset_x, uint16_t offset_y, uint16_t size_x, uint16_t *color_index, uint8_t *data)
{
  uint8_t data_to_send[50];

  for (int i = 0; i < 5; i++)
  {
    for (int j = 0; j < 5; j++)
    {
      data_to_send[(i + j * 5) * 2]     = color_index[data[x - 2 + i + ((y - 2 + j) * size_x)]] >> 8;
      data_to_send[(i + j * 5) * 2 + 1] = color_index[data[x - 2 + i + ((y - 2 + j) * size_x)]] & 0xFF;
    }
  }

  ILI9341_Set_Cursor_Position(hspi, x + offset_x - 2, y + offset_y - 2, x + offset_x + 2, y + offset_y + 2);

  ILI9341_Send_Command(hspi, ILI9341_MEM_WRITE);
  ILI9341_Send_Multiple_Data(hspi, data_to_send, 50);
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
  char speed_digit[4];

  sprintf(speed_digit, "%3d", speed);

  if (speed_digit[0] != previous_speed_digit[0])
  {
    cursor_x = SPEED_X;
    cursor_y = SPEED_Y;
    New_Draw_Char(hspi, previous_speed_digit[0], &SPEED_FONT, COLOR_BLACK);

    cursor_x = SPEED_X;
    cursor_y = SPEED_Y;
    New_Draw_Char(hspi, speed_digit[0], &SPEED_FONT, COLOR_WHITE);

    previous_speed_digit[0] = speed_digit[0];
  }

  if (speed_digit[1] != previous_speed_digit[1])
  {
    cursor_x = SPEED_X + SPEED_FONT_WIDTH;
    cursor_y = SPEED_Y;
    New_Draw_Char(hspi, previous_speed_digit[1], &SPEED_FONT, COLOR_BLACK);

    cursor_x = SPEED_X + SPEED_FONT_WIDTH;
    cursor_y = SPEED_Y;
    New_Draw_Char(hspi, speed_digit[1], &SPEED_FONT, COLOR_WHITE);

    previous_speed_digit[1] = speed_digit[1];
  }

  if (speed_digit[2] != previous_speed_digit[2])
  {
    cursor_x = SPEED_X + (2 * SPEED_FONT_WIDTH);
    cursor_y = SPEED_Y;
    New_Draw_Char(hspi, previous_speed_digit[2], &SPEED_FONT, COLOR_BLACK);

    cursor_x = SPEED_X + (2 * SPEED_FONT_WIDTH);
    cursor_y = SPEED_Y;
    New_Draw_Char(hspi, speed_digit[2], &SPEED_FONT, COLOR_WHITE);

    previous_speed_digit[2] = speed_digit[2];
  }
}

void ILI9341_Draw_Water_Temp(SPI_HandleTypeDef *hspi, int16_t temp)
{
  char temp_digit[7];

  sprintf(temp_digit, "%3d", temp);

  if (temp_digit[0] != previous_temp_digit[0])
  {
    cursor_x = TEMP_X;
    cursor_y = TEMP_Y;
    New_Draw_Char(hspi, previous_temp_digit[0], &TEMP_FONT, COLOR_BLACK);

    cursor_x = TEMP_X;
    cursor_y = TEMP_Y;
    New_Draw_Char(hspi, temp_digit[0], &TEMP_FONT, COLOR_WHITE);

    previous_temp_digit[0] = temp_digit[0];
  }

  if (temp_digit[1] != previous_temp_digit[1])
  {
    cursor_x = TEMP_X + TEMP_FONT_WIDTH;
    cursor_y = TEMP_Y;
    New_Draw_Char(hspi, previous_temp_digit[1], &TEMP_FONT, COLOR_BLACK);

    cursor_x = TEMP_X + TEMP_FONT_WIDTH;
    cursor_y = TEMP_Y;
    New_Draw_Char(hspi, temp_digit[1], &TEMP_FONT, COLOR_WHITE);

    previous_temp_digit[1] = temp_digit[1];
  }

  if (temp_digit[2] != previous_temp_digit[2])
  {
    cursor_x = TEMP_X;
    cursor_x = TEMP_X + (2 * TEMP_FONT_WIDTH);
    cursor_y = TEMP_Y;
    New_Draw_Char(hspi, previous_temp_digit[2], &TEMP_FONT, COLOR_BLACK);

    cursor_x = TEMP_X + (2 * TEMP_FONT_WIDTH);
    cursor_y = TEMP_Y;
    New_Draw_Char(hspi, temp_digit[2], &TEMP_FONT, COLOR_WHITE);

    previous_temp_digit[2] = temp_digit[2];
  }
}

void ILI9341_Draw_Intake_Temp(SPI_HandleTypeDef *hspi, int16_t itemp)
{
  char itemp_digit[7];

  sprintf(itemp_digit, "%3d", itemp);

  if (itemp_digit[0] != previous_itemp_digit[0])
  {
    cursor_x = ITEMP_X;
    cursor_y = ITEMP_Y;
    New_Draw_Char(hspi, previous_itemp_digit[0], &ITEMP_FONT, COLOR_BLACK);

    cursor_x = ITEMP_X;
    cursor_y = ITEMP_Y;
    New_Draw_Char(hspi, itemp_digit[0], &ITEMP_FONT, COLOR_WHITE);

    previous_itemp_digit[0] = itemp_digit[0];
  }

  if (itemp_digit[1] != previous_itemp_digit[1])
  {
    cursor_x = ITEMP_X + ITEMP_FONT_WIDTH;
    cursor_y = ITEMP_Y;
    New_Draw_Char(hspi, previous_itemp_digit[1], &ITEMP_FONT, COLOR_BLACK);

    cursor_x = ITEMP_X + ITEMP_FONT_WIDTH;
    cursor_y = ITEMP_Y;
    New_Draw_Char(hspi, itemp_digit[1], &ITEMP_FONT, COLOR_WHITE);

    previous_itemp_digit[1] = itemp_digit[1];
  }

  if (itemp_digit[2] != previous_itemp_digit[2])
  {
    cursor_x = ITEMP_X;
    cursor_x = ITEMP_X + (2 * ITEMP_FONT_WIDTH);
    cursor_y = ITEMP_Y;
    New_Draw_Char(hspi, previous_itemp_digit[2], &ITEMP_FONT, COLOR_BLACK);

    cursor_x = ITEMP_X + (2 * ITEMP_FONT_WIDTH);
    cursor_y = ITEMP_Y;
    New_Draw_Char(hspi, itemp_digit[2], &ITEMP_FONT, COLOR_WHITE);

    previous_itemp_digit[2] = itemp_digit[2];
  }
}

void ILI9341_Draw_Boost(SPI_HandleTypeDef *hspi, int16_t boost)
{
  if (boost <= 160 && boost >= -110)
  {
    ILI9341_Draw_Boost_Gauge_Pointer(hspi, GAUGE_X, GAUGE_Y, GAUGE_WIDTH, GAUGE_HEIGHT, (uint16_t *) opc_boost_gauge_148x148_color_index, (uint8_t *)opc_boost_gauge_148x148_data, boost);
  }

  char boost_string[7];
  char boost_digit[5];

  sprintf(boost_string, "%d", boost);

  if (boost < -99)
  {
    boost_digit[0] = boost_string[3];
    boost_digit[1] = boost_string[2];
    boost_digit[3] = boost_string[1];
    boost_digit[4] = boost_string[0];
  }
  else if (boost < -9)
  {
    boost_digit[0] = boost_string[2];
    boost_digit[1] = boost_string[1];
    boost_digit[3] = '0';
    boost_digit[4] = boost_string[0];
  }
  else if (boost < 0)
  {
    boost_digit[0] = boost_string[1];
    boost_digit[1] = '0';
    boost_digit[3] = '0';
    boost_digit[4] = boost_string[0];
  }
  else if (boost < 10)
  {
    boost_digit[0] = boost_string[0];
    boost_digit[1] = '0';
    boost_digit[3] = '0';
    boost_digit[4] = ' ';
  }
  else if (boost < 100)
  {
    boost_digit[0] = boost_string[1];
    boost_digit[1] = boost_string[0];
    boost_digit[3] = '0';
    boost_digit[4] = ' ';
  }
  else
  {
    boost_digit[0] = boost_string[2];
    boost_digit[1] = boost_string[1];
    boost_digit[3] = boost_string[0];
    boost_digit[4] = ' ';
  }

  if (boost_digit[0] != previous_boost_digit[0])
  {
    cursor_x = BOOST_X + + BOOST_FONT_DOT_WIDTH + 3 * BOOST_FONT_WIDTH;
    cursor_y = BOOST_Y;
    New_Draw_Char(hspi, previous_boost_digit[0], &BOOST_FONT, COLOR_BLACK);

    cursor_x = BOOST_X + + BOOST_FONT_DOT_WIDTH + 3 * BOOST_FONT_WIDTH;
    cursor_y = BOOST_Y;
    New_Draw_Char(hspi, boost_digit[0], &BOOST_FONT, COLOR_WHITE);

    previous_boost_digit[0] = boost_digit[0];
  }

  if (boost_digit[1] != previous_boost_digit[1])
  {
    cursor_x = BOOST_X + BOOST_FONT_DOT_WIDTH + 2 * BOOST_FONT_WIDTH;
    cursor_y = BOOST_Y;
    New_Draw_Char(hspi, previous_boost_digit[1], &BOOST_FONT, COLOR_BLACK);

    cursor_x = BOOST_X + BOOST_FONT_DOT_WIDTH + 2 * BOOST_FONT_WIDTH;
    cursor_y = BOOST_Y;
    New_Draw_Char(hspi, boost_digit[1], &BOOST_FONT, COLOR_WHITE);

    previous_boost_digit[1] = boost_digit[1];
  }

  if (boost_digit[3] != previous_boost_digit[3])
  {
    cursor_x = BOOST_X + BOOST_FONT_WIDTH;
    cursor_y = BOOST_Y;
    New_Draw_Char(hspi, previous_boost_digit[3], &BOOST_FONT, COLOR_BLACK);

    cursor_x = BOOST_X + BOOST_FONT_WIDTH;
    cursor_y = BOOST_Y;
    New_Draw_Char(hspi, boost_digit[3], &BOOST_FONT, COLOR_WHITE);

    previous_boost_digit[3] = boost_digit[3];
  }

  if (boost_digit[4] != previous_boost_digit[4])
  {
    cursor_x = BOOST_X;
    cursor_y = BOOST_Y;
    New_Draw_Char(hspi, previous_boost_digit[4], &BOOST_FONT, COLOR_BLACK);

    cursor_x = BOOST_X;
    cursor_y = BOOST_Y;
    New_Draw_Char(hspi, boost_digit[4], &BOOST_FONT, COLOR_WHITE);

    previous_boost_digit[4] = boost_digit[4];
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

void ILI9341_Draw_Boost_Gauge_Pointer(SPI_HandleTypeDef *hspi, uint16_t x, uint16_t y, uint16_t size_x, uint16_t size_y, uint16_t *color_index, uint8_t *data, int16_t boost)
{
  float angle = (float)boost * -0.0157079640776; // / 100 * 90 * -PI / 180

  uint16_t x0 = 85;
  uint16_t y0 = 74;
  uint16_t x1 = 13;
  uint16_t y1 = 74;

  float cos_x0 = (float)(x0 - 74) * cos(angle);
  float sin_x0 = (float)(x0 - 74) * sin(angle);
  float cos_y0 = (float)(y0 - 74) * cos(angle);
  float sin_y0 = (float)(y0 - 74) * sin(angle);
  float cos_x1 = (float)(x1 - 74) * cos(angle);
  float sin_x1 = (float)(x1 - 74) * sin(angle);
  float cos_y1 = (float)(y1 - 74) * cos(angle);
  float sin_y1 = (float)(y1 - 74) * sin(angle);

  x0 = cos_x0 + sin_y0 + 74;
  y0 = -sin_x0 + cos_y0 + 74;
  x1 = cos_x1 + sin_y1 + 74;
  y1 = -sin_x1 + cos_y1 + 74;

  previous_x0 = x0;
  previous_y0 = y0;
  previous_x1 = x1;
  previous_y1 = y1;

  memcpy(previous_gauge_pointer_pixel, gauge_pointer_pixel, sizeof(gauge_pointer_pixel));
  memset(gauge_pointer_pixel, 0, sizeof(gauge_pointer_pixel));

  ILI9341_Draw_Line(hspi, x + x0, y + y0, x + x1, y + y1, COLOR_OPC_RED);

  for (int i = 0; i < 512; i++)
  {
    if (gauge_pointer_pixel[i] != previous_gauge_pointer_pixel[i])
    {
      for (int j = 0; j < 32; j++)
      {
        if ((gauge_pointer_pixel[i] & (1 << j)) != (previous_gauge_pointer_pixel[i] & (1 << j)) &&
            (gauge_pointer_pixel[i] & (1 << j)) == 0)
        {
          int pixel = (i * 32) + j;
          uint16_t erase_x = GAUGE_POINTER_X + (pixel % GAUGE_POINTER_WIDTH);
          uint16_t erase_y = GAUGE_POINTER_Y + (pixel / GAUGE_POINTER_WIDTH);

          ILI9341_Draw_Pixel(hspi, erase_x, erase_y, color_index[data[erase_x - x + ((erase_y - y) * size_x)]]);
        }
      }
    }
  }
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

void ILI9341_Draw_Line(SPI_HandleTypeDef *hspi, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
  int16_t dx;
  int16_t dy;

  if (x0 <= x1)
  {
    dx = x1 - x0;
  }
  else
  {
    dx = x0 - x1;
  }
  if (y0 <= y1)
  {
    dy = y1 - y0;
  }
  else
  {
    dy = y0 - y1;
  }

  int16_t sx = (x0 < x1) ? 1 : -1;
  int16_t sy = (y0 < y1) ? 1 : -1;
  int16_t err = ((dx > dy) ? dx : -dy) / 2;
  int16_t e2;

  uint8_t toggle = 0;

  while (1) {
    toggle++;

    if (toggle == 1)
    {
      ILI9341_Draw_Large_Pixel(hspi, x0, y0, color);
    }
    else if (toggle == 2)
    {
      toggle = 0;
    }

    if (x0 == x1 && y0 == y1) {
      break;
    }

    e2 = err;
    if (e2 > -dx)
    {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dy)
    {
      err += dx;
      y0 += sy;
    }
  }
}

void ILI9341_Draw_Line_With_Data_Color(SPI_HandleTypeDef *hspi, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t offset_x, uint16_t offset_y, uint16_t size_x, uint16_t *color_index, uint8_t *data)
{
  int16_t dx;
  int16_t dy;

  if (x0 <= x1)
  {
    dx = x1 - x0;
  }
  else
  {
    dx = x0 - x1;
  }
  if (y0 <= y1)
  {
    dy = y1 - y0;
  }
  else
  {
    dy = y0 - y1;
  }

  int16_t sx = (x0 < x1) ? 1 : -1;
  int16_t sy = (y0 < y1) ? 1 : -1;
  int16_t err = ((dx > dy) ? dx : -dy) / 2;
  int16_t e2;

  uint8_t toggle = 0;

  while (1) {
    toggle++;

    if (toggle == 1)
    {
      ILI9341_Draw_Large_Pixel_With_Data_Color(hspi, x0, y0, offset_x, offset_y, size_x, color_index, data);
    }
    else if (toggle == 2)
    {
      toggle = 0;
    }

    if (x0 == x1 && y0 == y1) {
      break;
    }
    e2 = err;
    if (e2 > -dx) {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dy) {
      err += dx;
      y0 += sy;
    }
  }
}

static inline uint32_t Fetch_Bit(const uint8_t *p, uint32_t index)
{
  return (p[index >> 3] & (0x80 >> (index & 7)));
}

static uint32_t Fetch_Bits_Unsigned(const uint8_t *p, uint32_t index, uint32_t required)
{
  uint32_t val;
  uint8_t *s = (uint8_t *)&p[index>>3];

  val = s[0] << 24;
  val |= (s[1] << 16);
  val |= (s[2] << 8);
  val |= s[3];
  val <<= (index & 7); // shift out used bits

  if (32 - (index & 7) < required)
  {
    // need to get more bits
    val |= (s[4] >> (8 - (index & 7)));
  }

  val >>= (32 - required); // right align the bits
  return val;
}

static uint32_t Fetch_Bits_Signed(const uint8_t *p, uint32_t index, uint32_t required)
{
  uint32_t val = Fetch_Bits_Unsigned(p, index, required);

  if (val & (1 << (required - 1)))
  {
    return (int32_t)val - (1 << required);
  }

  return (int32_t)val;
}

void New_Draw_Char(SPI_HandleTypeDef *hspi, char c, const ILI9341_t3_font_t *font, uint16_t color)
{
  uint32_t bit_offset;
  const uint8_t *data;

  if (c >= font->index1_first && c <= font->index1_last)
  {
    bit_offset = c - font->index1_first;
    bit_offset *= font->bits_index;
  }
  else if (c >= font->index2_first && c <= font->index2_last)
  {
    bit_offset = c - font->index2_first + font->index1_last - font->index1_first + 1;
    bit_offset *= font->bits_index;
  }
  else
  {
    return;
  }

  data = font->data + Fetch_Bits_Unsigned(font->index, bit_offset, font->bits_index);
  bit_offset = 0;

  uint8_t encoding = Fetch_Bits_Unsigned(data, bit_offset, 3);
  bit_offset += 3;

  if (encoding != 0)
  {
    return;
  }

  uint16_t width = Fetch_Bits_Unsigned(data, bit_offset, font->bits_width);
  bit_offset += font->bits_width;

  uint16_t height = Fetch_Bits_Unsigned(data, bit_offset, font->bits_height);
  bit_offset += font->bits_height;

  int16_t xoffset = Fetch_Bits_Signed(data, bit_offset, font->bits_xoffset);
  bit_offset += font->bits_xoffset;

  int16_t yoffset = Fetch_Bits_Signed(data, bit_offset, font->bits_yoffset);
  bit_offset += font->bits_yoffset;

  uint16_t delta = Fetch_Bits_Unsigned(data, bit_offset, font->bits_delta);
  bit_offset += font->bits_delta;

  int16_t origin_x = cursor_x + xoffset;

  cursor_x += delta;

  int16_t origin_y = cursor_y + font->cap_height - height - yoffset;

  uint16_t line_count = height;

  uint16_t y = origin_y;

  while (line_count)
  {
    uint8_t b = Fetch_Bit(data, bit_offset++);

    if (b == 0)
    {
      // Do not repeat the line
      uint16_t x = 0;

      do
      {
        uint16_t x_size = width - x;

        if (x_size > 32)
        {
          x_size = 32;
        }

        uint32_t bits = Fetch_Bits_Unsigned(data, bit_offset, x_size);
        bit_offset += x_size;

        Draw_Font_Bits(hspi, bits, x_size, origin_x + x, y, 1, color);

        x += x_size;
      } while (x < width);

      y++;
      line_count--;
    }
    else
    {
      // Repeat the line n times
      uint8_t n = Fetch_Bits_Unsigned(data, bit_offset, 3) + 2;
      bit_offset += 3;

      uint16_t x = 0;

      do
      {
        uint16_t x_size = width - x;

        if (x_size > 32)
        {
          x_size = 32;
        }

        uint32_t bits = Fetch_Bits_Unsigned(data, bit_offset, x_size);
        bit_offset += x_size;

        Draw_Font_Bits(hspi, bits, x_size, origin_x + x, y, n, color);

        x += x_size;
      } while (x < width);

      y += n;
      line_count -= n;
    }
  }
}

void New_Draw_String(SPI_HandleTypeDef *hspi, char *str, const ILI9341_t3_font_t *font, uint16_t color)
{
  while (*str)
  {
    New_Draw_Char(hspi, *str++, font, color);
  }
}

void Draw_Font_Bits(SPI_HandleTypeDef *hspi, uint32_t bits, uint16_t num_bits, uint16_t x, uint16_t y, uint8_t repeat, uint16_t color)
{
  if (bits == 0)
  {
    return;
  }

  // left align bits
  bits <<= (32 - num_bits);

  uint16_t w;

  do
  {
    // skip over zeros
    w = __builtin_clz(bits);

    if (w > num_bits)
    {
      w = num_bits;
    }

    num_bits -= w;
    x += w;
    bits <<= w;

    // invert to count 1s as 0s
    bits = ~bits;
    w = __builtin_clz(bits);

    if (w > num_bits)
    {
      w = num_bits;
    }

    num_bits -= w;
    bits <<= w;

    // invert back to original polarity
    bits = ~bits;

    if (w > 0)
    {
      x += w;

      ILI9341_Fill_Rectangle(hspi, x - w, y, w, repeat, color);
    }
  } while (num_bits > 0);
}

uint16_t Get_Char_Width(char c, const ILI9341_t3_font_t *font)
{
  uint32_t bit_offset;
  const uint8_t *data;

  if (c >= font->index1_first && c <= font->index1_last)
  {
    bit_offset = c - font->index1_first;
    bit_offset *= font->bits_index;
  }
  else if (c >= font->index2_first && c <= font->index2_last)
  {
    bit_offset = c - font->index2_first + font->index1_last - font->index1_first + 1;
    bit_offset *= font->bits_index;
  }
  else
  {
    return 0;
  }

  data = font->data + Fetch_Bits_Unsigned(font->index, bit_offset, font->bits_index);
  bit_offset = 0;

  uint8_t encoding = Fetch_Bits_Unsigned(data, bit_offset, 3);

  if (encoding != 0)
  {
    return 0;
  }

  bit_offset += 3 + font->bits_width + font->bits_height + font->bits_xoffset + font->bits_yoffset;

  return Fetch_Bits_Unsigned(data, bit_offset, font->bits_delta);
}
