/**
 ******************************************************************************
 * @file         display.c
 * @author       Aaron Escoboza
 * @brief        Source file for the display feature
 ******************************************************************************
 */

#include "main.h"
#include "appConfig.h"
#include "bsp.h"
#include "guiImages.h"

#define DISPLAY_BEEP_DELAY                       100
#define NO_CLEAR_ON_ENTRY                        0
#define CLEAR_ALL_ON_EXIT                        0xffffffffUL
#define DISPLAY_MAX_SETTINGS_OPTIONS             3
#define DISPLAY_FEED_DELAY                       500
#define DISPLAY_RECT_INDICATOR_SIZE              20

/* Backlight macros*/
#define BACKLIGHT_NOT_WAIT                         0
#define BACKLIGHT_TIMER_ID                         1

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
    uint8_t backlightState;
}DisplaySettings;

typedef enum
{
    BACKLIGHT_ON,
    BACKLIGHT_OFF,
}BackLightState;

extern DispenserSettings dispenserSettings;
DisplaySettings displaySettings;
/* freeRTOS handlers */
TaskHandle_t xTaskDisplayHandler;
TimerHandle_t xBackLightTimerHandler;

void displayBacklightOn(void)
{
    HAL_GPIO_WritePin(TFT_LED_PORT, TFT_LED_PIN_NUM, GPIO_PIN_SET);
    displaySettings.backlightState = BACKLIGHT_ON;
}

void displayBackLightOff(void)
{
    HAL_GPIO_WritePin(TFT_LED_PORT, TFT_LED_PIN_NUM, GPIO_PIN_RESET);
    displaySettings.backlightState = BACKLIGHT_OFF;
}

BaseType_t displayRestartBacklightTimer(void)
{
    BaseType_t status = pdTRUE;

    status = xTimerReset(xBackLightTimerHandler, BACKLIGHT_NOT_WAIT);
    if (status != pdPASS)
    {
        status = pdFALSE;
        //TODO: Print error over a serial port.
    }
    return status;
}

void displaySetRecIndicatior(uint16_t x, uint16_t y, uint16_t w, Color color)
{
    /* Show cursor in the first rectangle */
    tft_ili9341_fill_rectangle(x - DISPLAY_RECT_INDICATOR_SIZE, y + (w / 2),
                               DISPLAY_RECT_INDICATOR_SIZE, DISPLAY_RECT_INDICATOR_SIZE, color);
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
    tft_ili9341_send_str(displaySettings.screenInit.settings.x + 25, displaySettings.screenInit.settings.y + 10,
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
                        buff, Font_16x26, WHITE, BLUE);
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
    tft_ili9341_send_str(SCREEN_SETTINGS_BACK_X + 60, SCREEN_SETTINGS_BACK_Y + 20,
                        "Back", Font_16x26, WHITE, ORANGE);
}

void backLightCallback(TimerHandle_t xTimer)
{
    displayBackLightOff();
}

BaseType_t displayBackLightInit(void)
{
    BaseType_t status = pdTRUE;

    /* Create one-shot timer to handle the backlight */
    xBackLightTimerHandler = xTimerCreate("backLight-timer", pdMS_TO_TICKS(DISPLAY_BACKLIGHT_DEFAULT_PERIOD), pdFALSE, (void*)BACKLIGHT_TIMER_ID, backLightCallback);
    if (xBackLightTimerHandler == NULL)
    {
        //handler no sufficient memory
        status = pdFALSE;
    }

    status = xTimerStart(xBackLightTimerHandler, BACKLIGHT_NOT_WAIT);
    if (status != pdPASS)
    {
        //TODO: Handle error
        status = pdFALSE;
    }
    displayBacklightOn();

    return status;
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
    displaySettings.screenInit.settings.x = 30;
    displaySettings.screenInit.settings.y = TFT_ILI9341_HEIGHT - 70;
    displaySettings.screenInit.settings.w = TFT_ILI9341_WIDTH - (2 * displaySettings.screenInit.settings.x);
    displaySettings.screenInit.settings.h = 40;
    // ILI9341_DrawImage((TFT_ILI9341_WIDTH - 240) / 2, (TFT_ILI9341_HEIGHT - 240) / 2, 240, 240, (const uint16_t *)firstScreen);
    /* Show the default screen when it starts */
    displayShowIniScreen();

}

void feed(uint8_t portions)
{
    int i;

    mspDisableButtonInterrupts();
    if ((portions == 0) || (portions > DISPENSER_MAX_PORTIONS))
    {
        dispenserBeep(100, 100, 3);
        return;
    }

    for (i = 0; i < portions; i++)
    {
        servoMotorRotate(SERVO_MOTOR_DEGREES_180);
        vTaskDelay(pdMS_TO_TICKS(DISPLAY_FEED_DELAY));
        servoMotorStop();
        vTaskDelay(pdMS_TO_TICKS(DISPLAY_FEED_DELAY));
        servoMotorRotate(SERVO_MOTOR_DEGREES_0);
        vTaskDelay(pdMS_TO_TICKS(DISPLAY_FEED_DELAY));
        servoMotorStop();
        vTaskDelay(pdMS_TO_TICKS(DISPLAY_FEED_DELAY));
    }
    mspEnableButtonInterrupts();
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
            if (dispenserSettings.portions > DISPENSER_MAX_PORTIONS)
            {
                dispenserSettings.portions = 1;
            }
            sprintf(buff, "%d", dispenserSettings.portions);
            /* Portion option */
            tft_ili9341_fill_rectangle(SCREEN_SETTINGS_PORTIONS_X, SCREEN_SETTINGS_PORTIONS_Y,
                                       SCREEN_SETTINGS_PORTIONS_W, SCREEN_SETTINGS_PORTIONS_H, BLUE);
            tft_ili9341_send_str(SCREEN_SETTINGS_PORTIONS_X + 75, SCREEN_SETTINGS_PORTIONS_Y + 20,
                                buff, Font_16x26, WHITE, BLUE);
        }
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionSelected == OPTION_SOUND))
        {
            /* Toggle the sound state*/
            dispenserSettings.sound ^= 1;
            (dispenserSettings.sound == DISPENSER_SOUND_ON) ? sprintf(buff, "ON") : sprintf(buff, "OFF");
            tft_ili9341_fill_rectangle(SCREEN_SETTINGS_SOUND_X, SCREEN_SETTINGS_SOUND_Y, SCREEN_SETTINGS_SOUND_W,
                                       SCREEN_SETTINGS_SOUND_H, BLUE);
            tft_ili9341_send_str(SCREEN_SETTINGS_SOUND_X + 75, SCREEN_SETTINGS_SOUND_Y + 20, buff, Font_16x26, WHITE, BLUE);
        }
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionSelected == OPTION_BACK))
        {
            dispenserBeep(100, 100, 1);
            displayShowIniScreen();
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
    BaseType_t status;
    uint32_t buttonEvent;
    uint8_t cursorPosition = OPTION_FEED;

    /* Initialize the backlight */
    status = displayBackLightInit();
    if (status != pdTRUE)
    {
        errorHandler();
    }
    /* Enable button interrupts */
    mspEnableButtonInterrupts();
    while (1)
    {
        /* Wait until a push button is pressed.
         * Notification settings: Index > 0, no clear on entry,
         *                       Clear all events on exit,
         *                       Block until there is an event.
         */
        xTaskNotifyWaitIndexed(BUTTON_INDEX_NOTIFICATION, NO_CLEAR_ON_ENTRY, CLEAR_ALL_ON_EXIT, &buttonEvent, portMAX_DELAY);
        status = displayRestartBacklightTimer();
        if (status != pdTRUE)
        {
            errorHandler();
        }
        if (displaySettings.backlightState == BACKLIGHT_OFF)
        {
            displayBacklightOn();
            continue;
        }

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
            cursorPosition = OPTION_FEED;
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
