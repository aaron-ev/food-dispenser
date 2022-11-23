
#ifndef __TFT_ILI9341_H
#define __TFT_ILI9341_H

#include "stm32f4xx_hal.h"
#include "fonts.h"
#include <stdint.h>

/* @GPIO definitions */
#define TFT_GPIOA                           GPIOA
#define TFT_GPIOB                           GPIOB
#define TFT_GPIOC                           GPIOC
#define TFT_GPIOD                           GPIOD
#define TFT_GPIOE                           GPIOE
#define TFT_GPIOF                           GPIOF
#define TFT_GPIOG                           GPIOG
#define TFT_GPIOH                           GPIOH

/* @Pin definitions */ 
#define TFT_PIN0                            GPIO_PIN_0
#define TFT_PIN1                            GPIO_PIN_1
#define TFT_PIN2                            GPIO_PIN_2
#define TFT_PIN3                            GPIO_PIN_3
#define TFT_PIN4                            GPIO_PIN_4
#define TFT_PIN5                            GPIO_PIN_5
#define TFT_PIN6                            GPIO_PIN_6
#define TFT_PIN7                            GPIO_PIN_7
#define TFT_PIN8                            GPIO_PIN_8
#define TFT_PIN9                            GPIO_PIN_9
#define TFT_PIN10                           GPIO_PIN_10
#define TFT_PIN11                           GPIO_PIN_11
#define TFT_PIN12                           GPIO_PIN_12
#define TFT_PIN13                           GPIO_PIN_13
#define TFT_PIN14                           GPIO_PIN_14
#define TFT_PIN15                           GPIO_PIN_15

/* @Alternate function definitions */
#define TFT_AF5_SPI1                        GPIO_AF5_SPI1
#define TFT_AF5_SPI2                        GPIO_AF5_SPI2
#define TFT_AF5_SPI3                        GPIO_AF5_SPI3
#define TFT_AF5_SPI4                        GPIO_AF5_SPI4
#define TFT_AF6_SPI2                        GPIO_AF6_SPI2
#define TFT_AF6_SPI3                        GPIO_AF6_SPI3
#define TFT_AF6_SPI4                        GPIO_AF6_SPI4
#define TFT_AF7_SPI2                        GPIO_AF7_SPI2
#define TFT_AF7_SPI3                        GPIO_AF7_SPI3


/* @SPI modules */
#define  TFT_SPI_MODULE_1                   SPI1
#define  TFT_SPI_MODULE_2                   SPI2
#define  TFT_SPI_MODULE_3                   SPI3
#define  TFT_SPI_MODULE_4                   SPI4

/* Max values of display */
#define TFT_ILI9341_MAX_X           240
#define TFT_ILI9341_MAX_Y           320

/************************************************************/
                 /* BEGIN OF USER CONFIGURATION */
/************************************************************/

/*
    For the following definition options check the references:

    @GPIO definitions
    @Pin definitions
    @Alternate function definitions
    @SPI modules
*/

/*
    Default settings: 
    CS    --> PB3
    RESET --> PB5
    DC    --> PB6
    LED   --> PB8
    MOSI  --> PA7
    MISO  --> PA6
    SCK   --> PA5
*/

/* CS */
#define TFT_CS_PORT                         TFT_GPIOB
#define TFT_CS_PIN_NUM                      TFT_PIN3

/* Reset */
#define TFT_RESET_PORT                      TFT_GPIOB
#define TFT_RESET_PIN_NUM                   TFT_PIN5

/* DC */
#define TFT_DC_PORT                         TFT_GPIOB
#define TFT_DC_PIN_NUM                      TFT_PIN6

/* LED */
#define TFT_LED_PORT                        TFT_GPIOB
#define TFT_LED_PIN_NUM                     TFT_PIN8

/* SPI section */
#define TFT_SPI_PORT                        TFT_GPIOA
#define TFT_SPI_MOSI_PIN_NUM                TFT_PIN7
#define TFT_SPI_MISO_PIN_NUM                TFT_PIN6
#define TFT_SPI_SCK_PIN_NUM                 TFT_PIN5
#define TFT_SPI_ALT                         TFT_AF5_SPI1
#define TFT_SPI_MODULE                      TFT_SPI_MODULE_1

/************************************************************/
                 /* END OF USER CONFIGURATION */
/************************************************************/

/* Command definitions */
#define TFT_CMD_NOP                         0x00
#define TFT_CMD_SOFTWARE_RESET              0x01
#define TFT_CMD_READ_DISPLAY_ID             0x04
#define TFT_CMD_READ_DISPLAY_STATUS         0x09
#define TFT_CMD_READ_DISPLAY_PWM            0x0A
#define TFT_CMD_READ_DISPLAY_MADCTL         0x0B
#define TFT_CMD_READ_PIXEL_FORMAT           0x0C
#define TFT_CMD_READ_DISPLAY_IMAGE_FORMAT   0x0D
#define TFT_CMD_READ_DISPLAY_SIGNAL_MODE    0x0E
#define TFT_CMD_READ_DIAGNOSTIC             0x0F
#define TFT_CMD_SLEEP_MODE                  0x10
#define TFT_CMD_SLEEP_OUT                   0x11
#define TFT_CMD_PARTIAL_MODE_ON             0x12
#define TFT_CMD_NORMAL_MODE                 0x13
#define TFT_CMD_INVERSION_OFF               0x20
#define TFT_CMD_INVERSION_ON                0x21
#define TFT_CMD_GAMMA_CURVE_SET             0x26
#define TFT_CMD_DISPLAY_OFF                 0x28
#define TFT_CMD_DISPLAY_ON                  0x29
#define TFT_CMD_COL_ADDRESS_SET             0x2A
#define TFT_CMD_PAGE_ADDRESS_SET            0x2B
#define TFT_CMD_MEM_WRITE                   0x2C
#define TFT_CMD_MEM_READ                    0x2D
#define TFT_CMD_COLOR_SET                   0x2E
#define TFT_CMD_PARTIAL_AREA                0x30
#define TFT_CMD_VER_SCROLLING               0x33
#define TFT_CMD_TEARING_EFFECT_OFF          0x34
#define TFT_CMD_TEARING_EFFECT_ON           0x35
#define TFT_CMD_MEM_ACCESS_CONTROL          0x36
#define TFT_CMD_VER_SCROLLING_START         0x37
#define TFT_CMD_IDLE_MODE_OFF               0x38
#define TFT_CMD_IDLE_MODE_ON                0x39
#define TFT_CMD_PIXEL_FORMAL_SET            0x3A
#define TFT_CMD_WRITE_MEM_CONTINUE          0x3C
#define TFT_CMD_READ_MEM_CONTINUE           0x3E
#define TFT_CMD_SET_TEAR_SCANLINE           0x44
#define TFT_CMD_GET_SCANLINE                0x45
#define TFT_CMD_WRITE_BRIGHTNESS            0x51
#define TFT_CMD_READ_BRIGHTNESS             0x52
#define TFT_CMD_WRITE_CTRL                  0x53
#define TFT_CMD_READ_CTRL                   0x54
#define TFT_CMD_WRITE_ADAPTIVE_BRIGHTNESS   0x55
#define TFT_CMD_READ_ADAPTIVE_BRIGHTNESS    0x56
#define TFT_CMD_WRITE_CABC_MIN_BRIGHTNESS   0x5E
#define TFT_CMD_READ_CABC_MIN_BRIGHTNESS    0x5F
#define TFT_CMD_READ_ID1                    0xDA
#define TFT_CMD_READ_ID2                    0xDB
#define TFT_CMD_READ_ID3                    0xDC
#define TFT_CMD_RGB_SIGN_CTRL               0xB0
#define TFT_CMD_FRAMER_CTRL_NORMAL          0xB1
#define TFT_CMD_FRAMER_CTRL_IDLE            0xB2
#define TFT_CMD_FRAMER_CTRL_PARTIAL         0xB3
#define TFT_CMD_INVERSION_CTRL              0xB4
#define TFT_CMD_BLANKING_PORCH_CTRL         0xB5
#define TFT_CMD_DISPLAY_FUNCTION_CTRL       0xB6
#define TFT_CMD_ENTRY_MODE_SET              0xB7
#define TFT_CMD_BACKLIGHT_CTRL1             0xB8
#define TFT_CMD_BACKLIGHT_CTRL2             0xB9
#define TFT_CMD_BACKLIGHT_CTRL3             0xBA
#define TFT_CMD_BACKLIGHT_CTRL4             0xBB
#define TFT_CMD_BACKLIGHT_CTRL5             0xBC
#define TFT_CMD_BACKLIGHT_CTRL7             0xBE
#define TFT_CMD_BACKLIGHT_CTRL8             0xBF
#define TFT_CMD_POWER_CTRL_VARH             0xC0
#define TFT_CMD_POWER_CTRL_SAP              0xC1
#define TFT_CMD_VCOM_CTRL1                  0xC5
#define TFT_CMD_VCOM_CTRL2                  0xC7
#define TFT_CMD_NV_MEM_WRITE                0xD0
#define TFT_CMD_NV_MEM_PROTECTION           0xD1
#define TFT_CMD_NV_MEM_STATUS               0xD2
#define TFT_CMD_READ_ID4                    0xD3
#define TFT_CMD_POS_GAMMA_CORRECTION        0xE0
#define TFT_CMD_NEG_GAMMA_CORRECTION        0xE1
#define TFT_CMD_DIG_GAMMA_CTRL1             0xE2
#define TFT_CMD_DIG_GAMMA_CTRL2             0xE3
#define TFT_CMD_INTERFACE_CTRL              0xF6
#define TFT_CMD_POWER_CTRLA                 0xCB
#define TFT_CMD_POWER_CTRLB                 0xCF
#define TFT_CMD_DRIV_TIMING_CTRLA           0xE8
#define TFT_CMD_DRIV_TIMING_CTRLB           0xEA
#define TFT_CMD_POWER_ON_SEQUENCE_CTRL      0xED
#define TFT_CMD_ENABLE_3G                   0xF2
#define TFT_CMD_PUMP_RATIO_CTRL             0xF7

typedef enum
{
    BLACK   = 0x0000,
    WHITE   = 0xFFFF,
    RED     = 0xF800,
    GREEN   = 0x07E0,
    GREEN2  = 0xB723,
    BLUE    = 0x001F,
    BLUE2   = 0x051D,
    YELLOW  = 0xFFE0,
    ORANGE  = 0xFBE4,
    CYAN    = 0x07FF,
    MAGENTA = 0xA254,
    GRAY    = 0x7BEF,
    BROWN   = 0xBBCA,
} Color;

#define TFT_ILI9341_WIDTH       240
#define TFT_ILI9341_HEIGHT      320

/*
 * Function name:
 * Description:
 */
void tft_ili9341_init(void);

/*
 * Function name:
 * Description:
 */
void tft_ili9341_display_on(void);

/*
 * Function name:
 * Description:
 */
void tft_ili9341_display_off(void);

/*
 * Function name:
 * Description:
 */
void tft_ili9341_draw_pixel(uint16_t x, uint16_t y, Color eColor);

/*
 * Function name:
 * Description:
 */
void tft_ili9341_fill_screen(Color eColor);


#endif

