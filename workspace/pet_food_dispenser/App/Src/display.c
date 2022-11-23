#include "stm32f4xx_hal.h"
#include "tft_ili9341.h"
#include "tft_ili9341_touch.h"
#include "testimg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "buzzer.h"

TaskHandle_t xTaskDisplayHandler;

void displayTouchInit(void)
{
    GPIO_InitTypeDef gpioTouchConfig  = {0};

    /* TOUCH IRQ pin settings */
    gpioTouchConfig.Pin = ILI9341_TOUCH_IRQ_PIN_NUM;
    gpioTouchConfig.Mode = GPIO_MODE_IT_FALLING;
    gpioTouchConfig.Pull = GPIO_NOPULL;
    gpioTouchConfig.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ILI9341_TOUCH_IRQ_GPIO_PORT, &gpioTouchConfig);

    /* TOUCH CS pin settings */
    gpioTouchConfig.Pin = ILI9341_TOUCH_CS_PIN_NUM;
    gpioTouchConfig.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(ILI9341_TOUCH_CS_GPIO_PORT, &gpioTouchConfig);

    /* Enable interrups */
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void displayInit(void)
{
    tft_ili9341_init();
//    displayTouchInit();
}

void displayWelcome(void)
{
    tft_ili9341_send_str(0, TFT_ILI9341_HEIGHT / 2, "Hello word, I am the display. Changing the background...", Font_16x26, BLUE, WHITE);
    HAL_Delay(1000);
    tft_ili9341_fill_screen(WHITE);
    tft_ili9341_send_str(0, TFT_ILI9341_HEIGHT / 2, "Writing an image...", Font_16x26, BLUE, WHITE);
    HAL_Delay(1000);
    ILI9341_DrawImage((TFT_ILI9341_WIDTH - 240) / 2, (TFT_ILI9341_HEIGHT - 240) / 2, 240, 240, (const uint16_t*)test_img_240x240);
    HAL_Delay(1000);
    tft_ili9341_fill_screen(WHITE);
    tft_ili9341_send_str(0, TFT_ILI9341_HEIGHT / 2, "Display: Ok", Font_16x26, BLUE, WHITE);
}


void vTaskDisplay(void *params)
{
    int i;
    for (i = 0; i < 2; i++)
    {
        buzzerPlay();
        HAL_Delay(500);
    }
    while (1)
    {  
        //TODO: 1. block until there is a new touch point 
        //      2. implement a FSM to handle the menu 
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* Handle for touch IQR line */
 void EXTI0_IRQHandler(void)
 {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
 }
