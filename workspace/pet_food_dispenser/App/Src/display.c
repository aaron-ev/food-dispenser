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
#include "main.h"

#define DISPLAY_BEEP_DELAY          100
#define NO_CLEAR_ON_ENTRY           0
#define CLEAR_ALL_ON_EXIT           0xffffffffUL

typedef enum
{
    OPTION_FEED,
    OPTION_SETTINGS,
}Options;

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

void feed(void)
{
    // TODO: update screen
    servoMotorRotate(SERVO_MOTOR_DEGREES_180);
    HAL_Delay(500);
    servoMotorRotate(SERVO_MOTOR_DEGREES_0);
}

void displayIniScreen(void)
{
    /* Show to recangules: 1. Feed, 2. Settings */
    /* Show cursos in the first rectangule */
}

void vTaskDisplay(void *params)
{
    uint32_t event;
    uint8_t cursorPosition = OPTION_FEED;

    buzzerBeep(100, 100, 2);
    testServoMotor();
    // displayIniScreen();
    while (1)
    {
        /* Wait until a push button is pressed.
         * Notification settings: Index > 0, no clear on entry,
         *                       Clear all events on exit,
         *                       Block until there is an event.
        */
        xTaskNotifyWaitIndexed(BUTTON_INDEX_NOTIFICATION, NO_CLEAR_ON_ENTRY, CLEAR_ALL_ON_EXIT, &event, portMAX_DELAY);
        if (event & BUTTON_EVENT_ENTER)
        {
            if (cursorPosition == OPTION_FEED)
            {
                feed();
            }
            if (cursorPosition == OPTION_SETTINGS)
            {
                //Go to screen for settings
                //settingsScreen();
            }
        }
        if (event & BUTTON_EVENT_UP)
        {
            if (cursorPosition == OPTION_FEED)
            {
                //There is no more options above, play a beep
                buzzerBeep(100, 100, 2);
            }
            else
            {
                cursorPosition = OPTION_FEED;
                 // TODO: update screen
            }
        }
        if (event & BUTTON_EVENT_DOWN)
        {
            if (cursorPosition == OPTION_SETTINGS)
            {
                buzzerBeep(100, 100, 2);
            }
            else
            {
                cursorPosition = OPTION_SETTINGS;
                 // TODO: update screen
            }
        }
    }
}
