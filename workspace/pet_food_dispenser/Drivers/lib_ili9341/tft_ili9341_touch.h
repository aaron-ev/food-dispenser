

#ifndef __TFT_ILI9341_TOUCH_H
#define __TFT_ILI9341_TOUCH_H

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stdint.h"

#define ILI9341_TOUCH_SPI_PORT		hspi1
extern SPI_HandleTypeDef ILI9341_TOUCH_SPI_PORT;

typedef enum
{
    TOUCH_NO_PRESSED,
    TOUCH_PRESSED
}TouchStatus;

#define ILI9341_TOUCH_IRQ_PIN_NUM           GPIO_PIN_0
#define ILI9341_TOUCH_IRQ_GPIO_PORT         GPIOC
#define ILI9341_TOUCH_CS_PIN_NUM            GPIO_PIN_1
#define ILI9341_TOUCH_CS_GPIO_PORT          GPIOC

#define ILI9341_TOUCH_WEIGHT                240
#define ILI9341_TOUCH_HEIGHT                320

TouchStatus ili9341_isTouchPressed(void);
TouchStatus ili93411_getTouchXY(uint16_t *x, uint16_t *y);

#endif
