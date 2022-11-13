#ifndef __ILI9341_H__
#define __ILI9341_H__

#include "fonts.h"
#include <stdbool.h>


/**************************************************************************
*                       Begin of  user configuration 
*
*           Default settings: 
*                           CS    --> PB3
*                           RESET --> PB5
*                           DC    --> PB6
*                           LED   --> PB8
*                           MOSI  --> PA7
*                           MISO  --> PA6
*                           SCK   --> PA5
*****************************************************************************/

/* CS */
#define ILI9341_CS_PORT                         GPIOB
#define ILI9341_CS_PIN_NUM                      GPIO_PIN_6

/* Reset */
#define ILI9341_RESET_PORT                      GPIOC
#define ILI9341_RESET_PIN_NUM                   GPIO_PIN_7

/* DC */
#define ILI9341_DC_PORT                         GPIOA
#define ILI9341_DC_PIN_NUM                      GPIO_PIN_9

/* LED */
#define ILI9341_LED_PORT                        GPIOB
#define ILI9341_LED_PIN_NUM                     GPIO_PIN_8

/* SPI section */
#define ILI9341_SPI_PORT                        GPIOA
#define ILI9341_SPI_MOSI_PIN_NUM                GPIO_PIN_7
#define ILI9341_SPI_MISO_PIN_NUM                GPIO_PIN_6
#define ILI9341_SPI_SCK_PIN_NUM                 GPIO_PIN_5
#define ILI9341_SPI_ALT                         GPIO_AF5_SPI1
#define ILI9341_SPI_MODULE                      SPI1

/****************************************************************************
*                       End of user configuration
*****************************************************************************/

extern SPI_HandleTypeDef hspi1;

#define ILI9341_SPI_HANDLE			    hspi1
#define ILI9341_MADCTL_MY               0x80
#define ILI9341_MADCTL_MX               0x40
#define ILI9341_MADCTL_MV               0x20
#define ILI9341_MADCTL_ML               0x10
#define ILI9341_MADCTL_RGB              0x00
#define ILI9341_MADCTL_BGR              0x08
#define ILI9341_MADCTL_MH               0x04

// #define ILI9341_RES_Pin       GPIO_PIN_7
// #define ILI9341_RES_GPIO_Port GPIOC

// #define ILI9341_CS_Pin        GPIO_PIN_6
// #define ILI9341_CS_GPIO_Port  GPIOB

// #define ILI9341_DC_Pin        GPIO_PIN_9
// #define ILI9341_DC_GPIO_Port  GPIOA

/**************************************************************************
*                           Default orientation
***************************************************************************/
#define ILI9341_WIDTH  240
#define ILI9341_HEIGHT 320
#define ILI9341_ROTATION (ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR)

// rotate right
/*
#define ILI9341_WIDTH  320
#define ILI9341_HEIGHT 240
#define ILI9341_ROTATION (ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR)
*/

// rotate left
/*
#define ILI9341_WIDTH  320
#define ILI9341_HEIGHT 240
#define ILI9341_ROTATION (ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR)
*/

// upside down
/*
#define ILI9341_WIDTH  240
#define ILI9341_HEIGHT 320
#define ILI9341_ROTATION (ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR)
*/

/**************************************************************************
*                           Color definitions
***************************************************************************/
#define	ILI9341_BLACK   0x0000
#define	ILI9341_BLUE    0x001F
#define	ILI9341_RED     0xF800
#define	ILI9341_GREEN   0x07E0
#define ILI9341_CYAN    0x07FF
#define ILI9341_MAGENTA 0xF81F
#define ILI9341_YELLOW  0xFFE0
#define ILI9341_WHITE   0xFFFF
#define ILI9341_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))


/**************************************************************************
*                           API declarations
***************************************************************************/
void ILI9341_Unselect();
void ILI9341_Init(void);
void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ILI9341_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);
void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ILI9341_FillScreen(uint16_t color);
void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
void ILI9341_InvertColors(bool invert);

#endif
