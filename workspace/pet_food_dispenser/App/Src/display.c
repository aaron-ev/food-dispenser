/**
 ******************************************************************************
 * @file         display.c
 * @author       Aaron Escoboza
 * @brief        Source file for the display feature
 ******************************************************************************
 */

#include "stm32f4xx_hal.h"
#include "tft_ili9341.h"
#include "testimg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "buzzer.h"
#include "servoMotor.h"
#include "appConfig.h"

#define DISPLAY_BEEP_DELAY          100

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

void testServoMotor(void)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        servoMotorRotate(SERVO_MOTOR_DEGREES_180);
        buzzerBeep(100, 0, 1);
        HAL_Delay(250);
        servoMotorRotate(SERVO_MOTOR_DEGREES_0);
        buzzerBeep(100, 0 , 1);
        HAL_Delay(250);
    }
}

void vTaskDisplay(void *params)
{
    buzzerBeep(100, 100, 2);
    testServoMotor();

    while (1)
    {
        //TODO: 1. block until there is a new touch point
        //      2. implement a FSM to handle the menu
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
