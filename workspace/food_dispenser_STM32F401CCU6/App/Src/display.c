/**
 ******************************************************************************
 * @file         display.c
 * @author       Aaron Escoboza
 * @brief        Source file for the display feature
 ******************************************************************************
 */

#include "main.h"
#include "appConfig.h"
#include "stm32f4xx_hal.h"
#include "tft_ili9341.h"
#include "testimg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "buzzer.h"
#include "servoMotor.h"

#define DISPLAY_BEEP_DELAY                       100
#define NO_CLEAR_ON_ENTRY                        0
#define CLEAR_ALL_ON_EXIT                        0xffffffffUL
#define DISPLAY_MAX_SETTINGS_OPTIONS             3
#define DISPLAY_FEED_DELAY                       500


/* Helper functions */
uint8_t getIndex(uint8_t option, uint8_t *arr);

extern DispenserSettings dispenserSettings;


/*
* Available options for the first screen.
*/
typedef enum
{
    OPTION_FEED,
    OPTION_SETTINGS,
} screenInitOptions;

/*
* Available options for the settings screen.
*/
typedef enum
{
    OPTION_PORTIONS,
    OPTION_SOUND,
    OPTION_BACK,
} screenSettingsOptions;

TaskHandle_t xTaskDisplayHandler;

void displayShowIniScreen(void)
{
    uint8_t rectFeedX = 30;
    uint8_t rectFeedY = TFT_ILI9341_HEIGHT / 2 - 60;
    uint8_t rectFeedW = TFT_ILI9341_WIDTH - (2 * rectFeedX);
    uint8_t rectFeedH = 60;
    uint8_t rectSettingsX = TFT_ILI9341_WIDTH / 2 - 20;
    uint8_t rectSettingsY = TFT_ILI9341_HEIGHT - 70;
    uint8_t rectSettingsW = TFT_ILI9341_WIDTH / 2 + 20;
    uint8_t rectSettingsH = 40;

    tft_ili9341_fill_screen(BLACK);
    /* Option to feed */
    tft_ili9341_fill_rectangle(rectFeedX, rectFeedY, rectFeedW, rectFeedH, ORANGE);
    tft_ili9341_send_str(rectFeedX + 50, rectFeedY + 20, "FEED", Font_16x26, WHITE, ORANGE);
    /* Option to change the settings */
    tft_ili9341_fill_rectangle(rectSettingsX, rectSettingsY, rectSettingsW, rectSettingsH, BLUE);
    tft_ili9341_send_str(rectSettingsX + 10, rectSettingsY + 10, "Settings", Font_16x26, WHITE, BLUE);
    /* Show cursor in the first rectangle */
    tft_ili9341_fill_rectangle(rectSettingsX - 10, rectSettingsY, 10, 10, GREEN);
}
void displayInit(void)
{
    /* Initialize the hardware and displays the default screen */
    tft_ili9341_init();
    displayShowIniScreen();
}

void displayWelcome(void)
{
    tft_ili9341_send_str(0, TFT_ILI9341_HEIGHT / 2, "Hello word, I am the display. Changing the background...", Font_16x26, BLUE, WHITE);
    HAL_Delay(1000);
    tft_ili9341_fill_screen(WHITE);
    tft_ili9341_send_str(0, TFT_ILI9341_HEIGHT / 2, "Writing an image...", Font_16x26, BLUE, WHITE);
    HAL_Delay(1000);
    ILI9341_DrawImage((TFT_ILI9341_WIDTH - 240) / 2, (TFT_ILI9341_HEIGHT - 240) / 2, 240, 240, (const uint16_t *)test_img_240x240);
    HAL_Delay(1000);
    tft_ili9341_fill_screen(WHITE);
    tft_ili9341_send_str(0, TFT_ILI9341_HEIGHT / 2, "Display: Ok", Font_16x26, BLUE, WHITE);
}

void feed(uint8_t portions)
{
    int i;

    if ((portions == 0) || (portions > DISPENSER_MAX_PORTIONS))
    {
        dispenserBeep(100, 100, 3);
        return;
    }

    for (i = 0; i < portions; i++)
    {
        servoMotorRotate(SERVO_MOTOR_DEGREES_180);
        HAL_Delay(DISPLAY_FEED_DELAY);
        servoMotorRotate(SERVO_MOTOR_DEGREES_0);
    }
}

uint8_t getIndex(uint8_t option, uint8_t *arr)
{
    int i;

    if (sizeof(arr) < 1)
    {
        return 0xFF;
    }
    for ( i = 0; i < sizeof(arr); i++)
    {
        if (arr[i] == arr[option])
        {
            break;
        }
    }

    return i;
}

void displayShowSettingsScreen(void)
{
    char portions[2];
    uint8_t rectFeedX = 30;
    uint8_t rectFeedY = TFT_ILI9341_HEIGHT / 2 - 60;
    uint8_t rectFeedW = TFT_ILI9341_WIDTH - (2 * rectFeedX);
    uint8_t rectFeedH = 60;
    uint8_t rectSettingsX = TFT_ILI9341_WIDTH / 2 - 20;
    uint8_t rectSettingsY = TFT_ILI9341_HEIGHT - 70;
    uint8_t rectSettingsW = TFT_ILI9341_WIDTH / 2 + 20;
    uint8_t rectSettingsH = 40;

    sprintf(portions, "%d", dispenserSettings.portions);

    tft_ili9341_fill_screen(BLACK);
    /* Show cursor in the first rectangle */
    tft_ili9341_fill_rectangle(rectSettingsX - 10, rectSettingsY, 10, 10, GREEN);
    /* Portion option */
    tft_ili9341_fill_rectangle(rectFeedX, rectFeedY, rectFeedW, rectFeedH, ORANGE);
    tft_ili9341_send_str(rectFeedX + 50, rectFeedY + 20, portions, Font_16x26, WHITE, ORANGE);
    /* Sound option */
    tft_ili9341_fill_rectangle(rectSettingsX, rectSettingsY, rectSettingsW, rectSettingsH, BLUE);
    if (dispenserSettings.sound == DISPENSER_SOUND_ON)
    {
        tft_ili9341_send_str(rectSettingsX + 10, rectSettingsY + 10, "ON", Font_16x26, WHITE, BLUE);
    }
    else
    {
        tft_ili9341_send_str(rectSettingsX + 10, rectSettingsY + 10, "OFF", Font_16x26, WHITE, BLUE);
    }
    //TODO: Show back rectangle
}

void screenSettings(void)
{
    uint32_t buttonEvent;
    uint8_t minOption = OPTION_BACK;
    uint8_t maxOption = OPTION_PORTIONS;
    uint8_t options [] = {OPTION_PORTIONS, OPTION_SOUND, OPTION_BACK};
    uint8_t optionIndex = getIndex(maxOption, options);
    uint8_t optionSelected = options[optionIndex];

    displayShowSettingsScreen();

    while (1)
    {
        /* Wait until a push button is pressed.
         * Notification settings: Index > 0, no clear on entry,
         *                       Clear all events on exit,
         *                       Block until there is an event.
         */
        xTaskNotifyWaitIndexed(BUTTON_INDEX_NOTIFICATION, NO_CLEAR_ON_ENTRY, CLEAR_ALL_ON_EXIT, &buttonEvent, portMAX_DELAY);
        /* Handle button ENTER event */
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionSelected == OPTION_PORTIONS))
        {
            dispenserSettings.portions++;
            if (dispenserSettings.portions == DISPENSER_MAX_PORTIONS)
            {
                dispenserSettings.portions = 1;
            }
            // TODO: update screen;
        }
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionSelected == OPTION_SOUND))
        {
            /* Toggle the sound state*/
            dispenserSettings.sound ^= 1;
            // TODO: update screen
        }
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionSelected == OPTION_BACK))
        {
            break;
        }
        /* Handle button UP event */
        if (buttonEvent & BUTTON_EVENT_UP && (optionSelected != maxOption))
        {
            optionSelected = options[optionIndex++];
        }
        if (buttonEvent & BUTTON_EVENT_UP && (optionSelected == maxOption))
        {
            dispenserBeep(50, 50, 2);
        }
        /* Handle button DOWN event */
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (optionSelected != minOption))
        {
            optionSelected = options[optionIndex--];
        }
        if (buttonEvent & BUTTON_EVENT_DOWN && (optionSelected == minOption))
        {
            dispenserBeep(50, 50, 2);
        }
    }
}

void vTaskDisplay(void *params)
{
    uint32_t buttonEvent;
    uint8_t cursorPosition = OPTION_FEED;

    while (1)
    {
        /* Wait until a push button is pressed.
         * Notification settings: Index > 0, no clear on entry,
         *                       Clear all events on exit,
         *                       Block until there is an event.
         */
        xTaskNotifyWaitIndexed(BUTTON_INDEX_NOTIFICATION, NO_CLEAR_ON_ENTRY, CLEAR_ALL_ON_EXIT, &buttonEvent, portMAX_DELAY);
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (cursorPosition == OPTION_FEED))
        {
            dispenserBeep(100, 100, 1);
            /* Update screen with a feed message */
            tft_ili9341_fill_screen(BLACK);
            tft_ili9341_send_str(TFT_ILI9341_WIDTH / 4 - 20, TFT_ILI9341_HEIGHT / 2, "Feeding...", Font_16x26, WHITE, BLACK);
            feed(dispenserSettings.portions);
            displayShowIniScreen();
        }
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (cursorPosition == OPTION_SETTINGS))
        {
            dispenserBeep(100, 100, 1);
            screenSettings();
        }
        if ((buttonEvent & BUTTON_EVENT_UP) && (cursorPosition & OPTION_FEED))
        {
            dispenserBeep(50, 50, 2);
        }
        if ((buttonEvent & BUTTON_EVENT_UP) && (cursorPosition & OPTION_FEED))
        {
            dispenserBeep(100, 100, 1);
            cursorPosition = OPTION_FEED;
            // TODO: update cursor position
        }
        if ((buttonEvent & BUTTON_EVENT_UP) && (cursorPosition & OPTION_FEED))
        {
            dispenserBeep(50, 50, 2);
        }
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (cursorPosition & OPTION_FEED))
        {
            dispenserBeep(100, 100, 1);
            cursorPosition = OPTION_SETTINGS;
            // TODO: update cursor position
        }
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (cursorPosition & OPTION_FEED))
        {
            dispenserBeep(50, 50, 2);
        }
    }
}
