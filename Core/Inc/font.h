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

extern FontTypeDef Font_7x10;
extern FontTypeDef Font_11x18;
extern FontTypeDef Font_16x26;
extern FontTypeDef Water_Temp_Logo_16x18;
extern FontTypeDef Degree_7x10;

#endif /* __FONT_H */
