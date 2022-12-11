#include "tft_ili9341.h"

#define ILI9341_WIDTH        TFT_ILI9341_WIDTH
#define ILI9341_HEIGHT       TFT_ILI9341_HEIGHT

#define UPPER_NIBBLE(n) n >> 8
#define LOWER_NIBBLE(n) n & 0xFF

typedef enum
{
    CLK_DISABLE,
    CLK_ENABLE
}eClkState;

typedef enum
{
    tft_ili9341_spi_8_mode,
    tft_ili9341_spi_16_mode,
}tft_ili9341_data_size;

/* Delay definitions */
#define TFT_DELAY_RESET          20
#define TFT_DELAY_DISPLAY_ON     100
#define TFD_DELAY_RESET          20

/* Global variables*/
SPI_HandleTypeDef hspi;

/* Functions prototypes */
void tft_ili9341_init(void);
static void tft_ili9341_gpio_init(void);
static void tft_ili9341_spi_init(void);
static void tft_ili9341_lcd_init(void);

static void tft_ili9341_send_cmd(uint8_t usCmd);
static void tft_ili9341_send_data(uint8_t* buff, size_t buff_len);
static void tft_ili9341_set_window(uint16_t x_col, uint16_t y_col, uint16_t x_page, uint16_t y_page);
void tft_ili9341_draw_pixel(uint16_t x, uint16_t y, Color eColor);
void tft_ili9341_fill_screen(Color eColor);

void tft_ili93241_select_chip(void)
{
    HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN_NUM, GPIO_PIN_RESET);
}

void tft_ili93241_unselect_chip(void)
{
    HAL_GPIO_WritePin(TFT_CS_PORT, TFT_CS_PIN_NUM, GPIO_PIN_SET);
}

void tft_ili9341_init(void)
{
    /* Initialize gpio pins according to user config */
    tft_ili9341_gpio_init();

    /* Initialize spi */
    tft_ili9341_spi_init();

    /* Initialize tft module */
    tft_ili9341_lcd_init();
}

static void tft_ili93241_gpio_clk_enable(GPIO_TypeDef *GPIOx, eClkState clkState)
{

    if (clkState == CLK_ENABLE)
    {
        if ( GPIOx == GPIOA)
        {
            __GPIOA_CLK_ENABLE();
        }
        else if ( GPIOx == GPIOB)
        {
            __GPIOB_CLK_ENABLE();
        }
        else if ( GPIOx == GPIOC)
        {
            __GPIOC_CLK_ENABLE();
        }
        else if ( GPIOx == GPIOE)
        {
            __GPIOE_CLK_ENABLE();
        }
        else
        {
            return;
        }
    }
    else
    {
        if ( GPIOx == GPIOA)
        {
            __GPIOA_CLK_DISABLE();
        }
        else if ( GPIOx == GPIOB)
        {
            __GPIOB_CLK_DISABLE();
        }
        else if ( GPIOx == GPIOC)
        {
            __GPIOC_CLK_DISABLE();
        }
        else if ( GPIOx == GPIOE)
        {
            __GPIOE_CLK_DISABLE();
        }
        else
        {
            return;
        }
    }
}


static void tft_ili93241_spi_clk_enable(SPI_TypeDef *SPIx, eClkState clkState)
{

    if (clkState == CLK_ENABLE)
    {
        if ( SPIx == SPI1)
        {
            __SPI1_CLK_ENABLE();
        }
        else if ( SPIx == SPI2 )
        {
            __SPI2_CLK_ENABLE();
        }
        else if ( SPIx == SPI3 )
        {
            __SPI3_CLK_ENABLE();
        }
        else if ( SPIx == SPI4 )
        {
            __SPI4_CLK_ENABLE();
        }
        else
        {
            return;
        }
    }
    else
    {
        if ( SPIx == SPI1)
        {
            __SPI1_CLK_DISABLE();
        }
        else if ( SPIx == SPI2 )
        {
            __SPI2_CLK_DISABLE();
        }
        else if ( SPIx == SPI3 )
        {
            __SPI3_CLK_DISABLE();
        }
        else if ( SPIx == SPI4 )
        {
            __SPI4_CLK_DISABLE();
        }
        else
        {
            return;
        }
    }
}

static void tft_ili9341_gpio_init(void)
{
    GPIO_InitTypeDef gpio_def = {0};

    /* Common settings for CS, RESET, DC and LED */
    gpio_def.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_def.Pull = GPIO_PULLUP;
    gpio_def.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    /* Initialize CS */
    gpio_def.Pin = TFT_CS_PIN_NUM;
    tft_ili93241_gpio_clk_enable(TFT_CS_PORT, CLK_ENABLE);
    HAL_GPIO_Init(TFT_CS_PORT, &gpio_def);

    /* Initialize RESET */
    gpio_def.Pin = TFT_RESET_PIN_NUM;
    tft_ili93241_gpio_clk_enable(TFT_RESET_PORT, CLK_ENABLE);
    HAL_GPIO_Init(TFT_RESET_PORT, &gpio_def);

    /* Initialize DC */
    gpio_def.Pin = TFT_DC_PIN_NUM;
    tft_ili93241_gpio_clk_enable(TFT_DC_PORT, CLK_ENABLE);
    HAL_GPIO_Init(TFT_DC_PORT, &gpio_def);

    /* Initialize LED */
    gpio_def.Pin = TFT_LED_PIN_NUM;
    tft_ili93241_gpio_clk_enable(TFT_LED_PORT, CLK_ENABLE);
    HAL_GPIO_Init(TFT_LED_PORT, &gpio_def);

    /* Initialize SPI pins (MOSI, MISO, CLK)*/
    gpio_def.Pin = TFT_SPI_MOSI_PIN_NUM | TFT_SPI_MISO_PIN_NUM | TFT_SPI_SCK_PIN_NUM;
    gpio_def.Mode = GPIO_MODE_AF_PP;
    gpio_def.Pull = GPIO_NOPULL;
    gpio_def.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_def.Alternate = TFT_SPI_ALT;
    tft_ili93241_gpio_clk_enable(TFT_SPI_PORT, CLK_ENABLE);
    HAL_GPIO_Init(TFT_SPI_PORT, &gpio_def);
}

static void tft_ili9341_spi_init(void)
{
    /* Master mode, half duplex, 8 bit data size, High polarity, 
       high phase (first edge), NSS managed by software and 
       LSB send first. 
    */
   tft_ili93241_spi_clk_enable(TFT_SPI_MODULE, CLK_ENABLE);
    hspi.Instance = TFT_SPI_MODULE;
    hspi.Init.Mode = SPI_MODE_MASTER;
    hspi.Init.Direction = SPI_DIRECTION_2LINES;
    hspi.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi.Init.NSS = SPI_NSS_SOFT;
    hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;

    if (HAL_SPI_Init(&hspi) != HAL_OK)
    {
        //TODO: handle the error
    }
}

static void tft_ili9341_lcd_init(void)
{
    /* Hardware reset */
    HAL_GPIO_WritePin(TFT_RESET_PORT, TFT_RESET_PIN_NUM, GPIO_PIN_RESET);
    HAL_Delay(TFD_DELAY_RESET);
    HAL_GPIO_WritePin(TFT_RESET_PORT, TFT_RESET_PIN_NUM, GPIO_PIN_SET);

    /* Select display chip */
    tft_ili93241_select_chip();

    /* Software reset */
    tft_ili9341_send_cmd(TFT_CMD_SOFTWARE_RESET);
    HAL_Delay(TFT_DELAY_RESET);

    /* Power control A: Setting default values */
    tft_ili9341_send_cmd(TFT_CMD_POWER_CTRLA);
    {
        uint8_t data[] = {0x39, 0x2C, 0x00, 0x34, 0x02};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* Power control B*/
    tft_ili9341_send_cmd(TFT_CMD_POWER_CTRLB);
    {
        uint8_t data[] = {0x00, 0xC1, 0x30};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* Driving timing control A*/
    tft_ili9341_send_cmd(TFT_CMD_DRIV_TIMING_CTRLA);
    {
        uint8_t data[] = {0x85, 0x00, 0x78};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* Driving timing control B */
    tft_ili9341_send_cmd(TFT_CMD_DRIV_TIMING_CTRLB);
    {
        uint8_t data[] = {0x00, 0x00};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* Power on sequence control */
    tft_ili9341_send_cmd(TFT_CMD_POWER_ON_SEQUENCE_CTRL);
    {
        uint8_t data[] = {0x64, 0x03, 0x12, 0x81};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* Pump ratio control */
    tft_ili9341_send_cmd(TFT_CMD_PUMP_RATIO_CTRL);
    {
        uint8_t data[] = {0x20};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* Power control VRH */
    tft_ili9341_send_cmd(TFT_CMD_POWER_CTRL_VARH);
    {
        uint8_t data[] = {0x23};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* Power control SAP */
    tft_ili9341_send_cmd(TFT_CMD_POWER_CTRL_SAP);
    {
        uint8_t data[] = {0x10};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* VCM control 1 */
    tft_ili9341_send_cmd(TFT_CMD_VCOM_CTRL1);
    {
        uint8_t data[] = {0x3E, 0x28};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* VCM control 2 */
    tft_ili9341_send_cmd(TFT_CMD_VCOM_CTRL2);
    {
        uint8_t data[] = {0xC7};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* Memory access control */
    tft_ili9341_send_cmd(TFT_CMD_MEM_ACCESS_CONTROL);
    {
        uint8_t data[] = {0x48};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* Pixel format */
    tft_ili9341_send_cmd(TFT_CMD_PIXEL_FORMAL_SET);
    {
        uint8_t data[] = {0x55};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* Frame ratio control - standard RGB color */
    tft_ili9341_send_cmd(TFT_CMD_FRAMER_CTRL_NORMAL);
    {
        uint8_t data[] = {0x00, 0x18};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* Display function control */
    tft_ili9341_send_cmd(TFT_CMD_DISPLAY_FUNCTION_CTRL);
    {
        uint8_t data[] = {0x08, 0x82, 0x27};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* 3 gamma function disable */
    tft_ili9341_send_cmd(TFT_CMD_ENABLE_3G);
    {
        uint8_t data[] = {0x00};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* gamma curve selected */
    tft_ili9341_send_cmd(TFT_CMD_GAMMA_CURVE_SET);
    {
        uint8_t data[] = {0x01};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* Positive gamma correction */
    tft_ili9341_send_cmd(TFT_CMD_POS_GAMMA_CORRECTION);
    {
        uint8_t data[] = {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x8, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* Negative gamma correction */
    tft_ili9341_send_cmd(TFT_CMD_NEG_GAMMA_CORRECTION);
    {
        uint8_t data[] = {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F};
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* Sleep out and turn on the display */
    tft_ili9341_send_cmd(TFT_CMD_SLEEP_OUT);
    HAL_Delay(TFT_DELAY_DISPLAY_ON);
    tft_ili9341_send_cmd(TFT_CMD_DISPLAY_ON);
    tft_ili9341_send_cmd(TFT_CMD_MEM_WRITE);

    tft_ili93241_unselect_chip();
}

static void tft_ili9341_send_cmd(uint8_t usCmd)
{
    HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN_NUM, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi, &usCmd, 1, HAL_MAX_DELAY);
}

static void tft_ili9341_send_data(uint8_t* buff, size_t buff_len)
{
    HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN_NUM, GPIO_PIN_SET);
    while (buff_len > 0)
    {
        uint16_t chunk_len = buff_len  > 32768 ? 32768 : buff_len ;
        HAL_SPI_Transmit(&hspi, buff, chunk_len, HAL_MAX_DELAY);
        buff_len -= chunk_len;
        buff += chunk_len;
    }
}
void tft_ili9341_display_on(void)
{
    tft_ili9341_send_cmd(TFT_CMD_DISPLAY_ON);
}

void tft_ili9341_display_off(void)
{
    tft_ili9341_send_cmd(TFT_CMD_DISPLAY_OFF);
}

static void tft_ili9341_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) 
{
    /* column address set */
    tft_ili9341_send_cmd(0x2A);
    {
        uint8_t data[] = { (x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF };
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* row address set */
    tft_ili9341_send_cmd(0x2B);
    {
        uint8_t data[] = { (y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF };
        tft_ili9341_send_data(data, sizeof(data));
    }

    /* write to RAM */
    tft_ili9341_send_cmd(0x2C);
}

void tft_ili9341_fill_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) 
{
    uint8_t data[] = { UPPER_NIBBLE(color), LOWER_NIBBLE(color) };

    if ( (x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) 
    {
        return;
    }
        
    if ( (x + w - 1) >= ILI9341_WIDTH)
    {
        w = ILI9341_WIDTH - x;
    }

    if ( (y + h - 1) >= ILI9341_HEIGHT)
    {
        h = ILI9341_HEIGHT - y;
    }

    tft_ili93241_select_chip();
    tft_ili9341_set_window(x, y, x+w-1, y+h-1);
    HAL_GPIO_WritePin(TFT_DC_PORT, TFT_DC_PIN_NUM, GPIO_PIN_SET);
    for ( y = h; y > 0; y-- )
    {
        for ( x = w; x > 0; x-- )
        {
            HAL_SPI_Transmit(&hspi, data, sizeof(data), HAL_MAX_DELAY);
        }
    }
    tft_ili93241_unselect_chip();
}

void tft_ili9341_fill_screen(Color eColor)
{
    tft_ili9341_fill_rectangle(0, 0, TFT_ILI9341_WIDTH, TFT_ILI9341_HEIGHT, eColor);
}

void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data)
{
    if ( (x >= TFT_ILI9341_WIDTH) || (y >= TFT_ILI9341_HEIGHT) )
    {
        return;
    }
    if ( (x + w - 1) >= TFT_ILI9341_WIDTH)
    {
        return;
    }
    tft_ili93241_select_chip();
    tft_ili9341_set_window(x, y, x+w-1, y+h-1);
    tft_ili9341_send_data( (uint8_t*)data, sizeof(uint16_t) * w * h);
    tft_ili93241_unselect_chip();
}


static void tft_ili9341_send_char(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) 
{
    uint32_t i, b, j;

    tft_ili9341_set_window(x, y, x+font.width-1, y+font.height-1);
    for(i = 0; i < font.height; i++) 
    {
        b = font.data[(ch - 32) * font.height + i];
        for (j = 0; j < font.width; j++) 
        {
            if ((b << j) & 0x8000)
            {
                uint8_t data[] = { color >> 8, color & 0xFF };
                tft_ili9341_send_data(data, sizeof(data));
            } 
            else 
            {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                tft_ili9341_send_data(data, sizeof(data));
            }
        }
    }
}

void tft_ili9341_send_str(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor)
{
    tft_ili93241_select_chip();
    while (*str)
    {
        if (x + font.width >= ILI9341_WIDTH)
        {
            x = 0;
            y += font.height;
            if (y + font.height >= ILI9341_HEIGHT) 
            {
                break;
            }

            if (*str == ' ')
            {
                /* skip spaces in the beginning of the new line */
                str++;
                continue;
            }
        }
        tft_ili9341_send_char(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }
    tft_ili93241_unselect_chip();
}
