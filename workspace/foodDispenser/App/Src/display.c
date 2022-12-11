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
#include "FreeRTOS.h"
#include "task.h"
#include "buzzer.h"
#include "servoMotor.h"
#include "cat.h"
#include "msp.h"

#define DISPLAY_BEEP_DELAY                       100
#define NO_CLEAR_ON_ENTRY                        0
#define CLEAR_ALL_ON_EXIT                        0xffffffffUL
#define DISPLAY_MAX_SETTINGS_OPTIONS             3
#define DISPLAY_FEED_DELAY                       500
#define DISPLAY_RECT_INDICADOR_SIZE              20


/* Coordinates for the settings screen */
#define SCREEN_SETTINGS_PORTIONS_X         30
#define SCREEN_SETTINGS_PORTIONS_Y         (TFT_ILI9341_HEIGHT / 2 - 90)
#define SCREEN_SETTINGS_PORTIONS_W         (TFT_ILI9341_WIDTH - (2 * SCREEN_SETTINGS_PORTIONS_X))
#define SCREEN_SETTINGS_PORTIONS_H         60

#define SCREEN_SETTINGS_SOUND_X            30
#define SCREEN_SETTINGS_SOUND_Y            (TFT_ILI9341_HEIGHT / 2 - 20)
#define SCREEN_SETTINGS_SOUND_W            (TFT_ILI9341_WIDTH - (2 * SCREEN_SETTINGS_SOUND_X))
#define SCREEN_SETTINGS_SOUND_H            60

#define SCREEN_SETTINGS_BACK_X             30
#define SCREEN_SETTINGS_BACK_Y             (TFT_ILI9341_HEIGHT - 70)
#define SCREEN_SETTINGS_BACK_W             (TFT_ILI9341_WIDTH - (2 * SCREEN_SETTINGS_BACK_X))
#define SCREEN_SETTINGS_BACK_H             60

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

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
}Rectangle;

typedef struct
{
    Rectangle feed;
    Rectangle settings;
}ScreenInit;

typedef struct
{
    Rectangle portions;
    Rectangle sound;
    Rectangle back;
}ScreenSettings;

typedef struct
{
    Color backgroundColor;
    ScreenInit screenInit;
    ScreenSettings screenSettings;
}DisplaySettings;

DisplaySettings displaySettings;
TaskHandle_t xTaskDisplayHandler;

void displayBacklightOn(void)
{
    HAL_GPIO_WritePin(TFT_LED_PORT, TFT_LED_PIN_NUM, GPIO_PIN_SET);
}

void displayBackLightOff(void)
{
    HAL_GPIO_WritePin(TFT_LED_PORT, TFT_LED_PIN_NUM, GPIO_PIN_RESET);
}

void displaySetRecIndicatior(uint16_t x, uint16_t y, uint16_t w, Color color)
{
    /* Show cursor in the first rectangle */
    tft_ili9341_fill_rectangle(x - DISPLAY_RECT_INDICADOR_SIZE, y + (w / 2),
                               DISPLAY_RECT_INDICADOR_SIZE, DISPLAY_RECT_INDICADOR_SIZE, color);
}

void displayShowIniScreen(void)
{
    tft_ili9341_fill_screen(displaySettings.backgroundColor);
    /* Option to feed */
    tft_ili9341_fill_rectangle(displaySettings.screenInit.feed.x, displaySettings.screenInit.feed.y,
                               displaySettings.screenInit.feed.w, displaySettings.screenInit.feed.h, ORANGE);
    tft_ili9341_send_str(displaySettings.screenInit.feed.x + 50, displaySettings.screenInit.feed.y + 20, "Feed",
                         Font_16x26, WHITE, ORANGE);
    /* Option to change the settings */
    tft_ili9341_fill_rectangle(displaySettings.screenInit.settings.x, displaySettings.screenInit.settings.y,
                               displaySettings.screenInit.settings.w, displaySettings.screenInit.settings.h, BLUE);
    tft_ili9341_send_str(displaySettings.screenInit.settings.x + 10, displaySettings.screenInit.settings.y + 10,
                         "Settings", Font_16x26, WHITE, BLUE);
    displaySetRecIndicatior(displaySettings.screenInit.feed.x, displaySettings.screenInit.feed.y,
                            displaySettings.screenInit.feed.h, GREEN);
}

void displayShowSettingsScreen(void)
{
    char buff[4];

    sprintf(buff, "%d", dispenserSettings.portions);
    /* Fill the screen with the background color */
    tft_ili9341_fill_screen(displaySettings.backgroundColor);
    /* Portion option */
    tft_ili9341_fill_rectangle(SCREEN_SETTINGS_PORTIONS_X, SCREEN_SETTINGS_PORTIONS_Y,
                               SCREEN_SETTINGS_PORTIONS_W, SCREEN_SETTINGS_PORTIONS_H, BLUE);
    tft_ili9341_send_str(SCREEN_SETTINGS_PORTIONS_X + 75, SCREEN_SETTINGS_PORTIONS_Y + 20,
                        buff, Font_16x26, WHITE, ORANGE);
    /* Sound option */
    tft_ili9341_fill_rectangle(SCREEN_SETTINGS_SOUND_X, SCREEN_SETTINGS_SOUND_Y, SCREEN_SETTINGS_SOUND_W,
                               SCREEN_SETTINGS_SOUND_H, BLUE);
    /* Show sound state */
    (dispenserSettings.sound == DISPENSER_SOUND_ON) ? sprintf(buff, "ON") : sprintf(buff, "OFF");
    tft_ili9341_send_str(SCREEN_SETTINGS_SOUND_X + 75, SCREEN_SETTINGS_SOUND_Y + 20, buff, Font_16x26, WHITE, BLUE);
    /* Show cursor in the first rectangle */
    displaySetRecIndicatior(SCREEN_SETTINGS_PORTIONS_X, SCREEN_SETTINGS_PORTIONS_Y, SCREEN_SETTINGS_PORTIONS_H
                            , GREEN);
    /* Back option */
    tft_ili9341_fill_rectangle(SCREEN_SETTINGS_BACK_X, SCREEN_SETTINGS_BACK_Y, SCREEN_SETTINGS_BACK_W,
                               SCREEN_SETTINGS_BACK_H, ORANGE);
}

void displayInit(void)
{
    /* Initialize the hardware and display with default settings */
    tft_ili9341_init();
    displaySettings.backgroundColor = BLACK;
    /* Start screen */
    displaySettings.screenInit.feed.x = 30;
    displaySettings.screenInit.feed.y = TFT_ILI9341_HEIGHT / 2 - 60;
    displaySettings.screenInit.feed.w = TFT_ILI9341_WIDTH - (2 * displaySettings.screenInit.feed.x);
    displaySettings.screenInit.feed.h = 2 * displaySettings.screenInit.feed.x;
    displaySettings.screenInit.settings.x = TFT_ILI9341_WIDTH / 2 - 20;
    displaySettings.screenInit.settings.y = TFT_ILI9341_HEIGHT - 70;
    displaySettings.screenInit.settings.w = TFT_ILI9341_WIDTH / 2 + 20;
    displaySettings.screenInit.settings.h = 40;
    ILI9341_DrawImage((TFT_ILI9341_WIDTH - 240) / 2, (TFT_ILI9341_HEIGHT - 240) / 2, 240, 240, (const uint16_t *)snk);
    displayShowIniScreen();
}

// void displayWelcome(void)
// {
//     tft_ili9341_send_str(0, TFT_ILI9341_HEIGHT / 2, "Hello word, I am the display. Changing the background...", Font_16x26, BLUE, WHITE);
//     HAL_Delay(1000);
//     tft_ili9341_fill_screen(WHITE);
//     tft_ili9341_send_str(0, TFT_ILI9341_HEIGHT / 2, "Writing an image...", Font_16x26, BLUE, WHITE);
//     HAL_Delay(1000);
// //    ILI9341_DrawImage((TFT_ILI9341_WIDTH - 240) / 2, (TFT_ILI9341_HEIGHT - 240) / 2, 240, 240, (const uint16_t *)test_img_240x240);
//     HAL_Delay(1000);
//     tft_ili9341_fill_screen(WHITE);
//     tft_ili9341_send_str(0, TFT_ILI9341_HEIGHT / 2, "Display: Ok", Font_16x26, BLUE, WHITE);
// }

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

void screenSettings(void)
{
    char buff[4];
    uint32_t buttonEvent;
    uint8_t options [] = {OPTION_BACK, OPTION_SOUND, OPTION_PORTIONS};
    uint8_t minOption= options[0];
    uint8_t maxOption= options[sizeof(options) - 1];
    uint8_t index = sizeof(options) - 1; /* Max index by default */
    uint8_t optionSelected = options[index];

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
            (dispenserSettings.sound == DISPENSER_SOUND_ON) ? sprintf(buff, "ON") : sprintf(buff, "OFF");
            tft_ili9341_send_str(SCREEN_SETTINGS_SOUND_X + 75, SCREEN_SETTINGS_SOUND_Y + 20, buff, Font_16x26, WHITE, BLUE);
        }
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionSelected == OPTION_BACK))
        {
            break;
        }
        /* Handle button UP event */
        if (buttonEvent & BUTTON_EVENT_UP && (optionSelected != maxOption))
        {
            optionSelected = options[++index];
        }
        if (buttonEvent & BUTTON_EVENT_UP && (optionSelected == maxOption))
        {
            dispenserBeep(50, 50, 2);
        }
        /* Handle button DOWN event */
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (optionSelected != minOption))
        {
            optionSelected = options[--index];
        }
        if (buttonEvent & BUTTON_EVENT_DOWN && (optionSelected == minOption))
        {
            dispenserBeep(50, 50, 2);
        }
        /*Update cursor */
        tft_ili9341_fill_rectangle(0, 0, 30, TFT_ILI9341_HEIGHT, displaySettings.backgroundColor);
        if (optionSelected == OPTION_PORTIONS)
        {
            displaySetRecIndicatior(SCREEN_SETTINGS_PORTIONS_X, SCREEN_SETTINGS_PORTIONS_Y, SCREEN_SETTINGS_PORTIONS_H, GREEN);
        }
        if (optionSelected == OPTION_SOUND)
        {
            displaySetRecIndicatior(SCREEN_SETTINGS_SOUND_X, SCREEN_SETTINGS_SOUND_Y, SCREEN_SETTINGS_SOUND_H, GREEN);
        }
        if (optionSelected == OPTION_BACK)
        {
            displaySetRecIndicatior(SCREEN_SETTINGS_BACK_X, SCREEN_SETTINGS_BACK_Y, SCREEN_SETTINGS_BACK_H, GREEN);
        }
    }
}

void vTaskDisplay(void *params)
{
    uint32_t buttonEvent;
    uint8_t cursorPosition = OPTION_FEED;
    /* Enable interrupts */
    mspEnableButtonInterrupts();
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
            dispenserBeep(100, 100, 3);
            screenSettings();
        }
        if ((buttonEvent & BUTTON_EVENT_UP) && (cursorPosition == OPTION_FEED))
        {
            dispenserBeep(50, 50, 2);
        }
        if ((buttonEvent & BUTTON_EVENT_UP) && (cursorPosition == OPTION_SETTINGS))
        {
            displaySetRecIndicatior(displaySettings.screenInit.feed.x, displaySettings.screenInit.feed.y,
                            displaySettings.screenInit.feed.h, GREEN);
            displaySetRecIndicatior(displaySettings.screenInit.settings.x, displaySettings.screenInit.settings.y,
                            displaySettings.screenInit.settings.h, displaySettings.backgroundColor);
            dispenserBeep(100, 100, 1);
            cursorPosition = OPTION_FEED;
        }
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (cursorPosition == OPTION_FEED))
        {
            displaySetRecIndicatior(displaySettings.screenInit.feed.x, displaySettings.screenInit.feed.y,
                            displaySettings.screenInit.feed.h, displaySettings.backgroundColor);
            displaySetRecIndicatior(displaySettings.screenInit.settings.x, displaySettings.screenInit.settings.y,
                            displaySettings.screenInit.settings.h, GREEN);
            dispenserBeep(100, 100, 1);
            cursorPosition = OPTION_SETTINGS;
        }
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (cursorPosition == OPTION_SETTINGS))
        {
            dispenserBeep(50, 50, 2);
        }
    }
}
