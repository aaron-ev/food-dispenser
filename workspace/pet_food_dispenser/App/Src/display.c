#include "stm32f4xx_hal.h"
#include "tft_ili9341.h"
#include "testimg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "buzzer.h"
#include "appConfig.h"

TaskHandle_t xTaskDisplayHandler;

void displayInit(void)
{
    tft_ili9341_init();
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
        buzzerStart(BUZZER_TIM_CHANNEL);
        HAL_Delay(500);
        buzzerStop(BUZZER_TIM_CHANNEL);
    }

    while (1)
    {
        //TODO: 1. block until there is a new touch point
        //      2. implement a FSM to handle the menu
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
