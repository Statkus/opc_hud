/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FONT_H
#define __FONT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private defines -----------------------------------------------------------*/
typedef struct {
  uint8_t width;
  uint8_t height;
  uint16_t *data;
} FontTypeDef;

//extern FontTypeDef Font_7x10;
//extern FontTypeDef Font_11x18;
//extern const FontTypeDef Font_16x26;
extern FontTypeDef Water_Temp_Logo_16x18;
extern FontTypeDef Degree_7x10;

typedef struct {
  const unsigned char *index;
  const unsigned char *unicode;
  const unsigned char *data;
  unsigned char version;
  unsigned char reserved;
  unsigned char index1_first;
  unsigned char index1_last;
  unsigned char index2_first;
  unsigned char index2_last;
  unsigned char bits_index;
  unsigned char bits_width;
  unsigned char bits_height;
  unsigned char bits_xoffset;
  unsigned char bits_yoffset;
  unsigned char bits_delta;
  unsigned char line_space;
  unsigned char cap_height;
} ILI9341_t3_font_t;

//extern const ILI9341_t3_font_t Arial_8;
extern const ILI9341_t3_font_t Arial_12;
extern const ILI9341_t3_font_t Arial_12_Bold;
extern const ILI9341_t3_font_t Arial_18_Bold;
extern const ILI9341_t3_font_t Arial_40_Bold;

//extern const ILI9341_t3_font_t ArialBlack_8;
//extern const ILI9341_t3_font_t ArialBlack_9;
//extern const ILI9341_t3_font_t ArialBlack_10;
//extern const ILI9341_t3_font_t ArialBlack_11;
extern const ILI9341_t3_font_t ArialBlack_12;
//extern const ILI9341_t3_font_t ArialBlack_13;
//extern const ILI9341_t3_font_t ArialBlack_14;
//extern const ILI9341_t3_font_t ArialBlack_16;
//extern const ILI9341_t3_font_t ArialBlack_18;
//extern const ILI9341_t3_font_t ArialBlack_20;
//extern const ILI9341_t3_font_t ArialBlack_24;
//extern const ILI9341_t3_font_t ArialBlack_28;
//extern const ILI9341_t3_font_t ArialBlack_32;
extern const ILI9341_t3_font_t ArialBlack_40;

#endif /* __FONT_H */
