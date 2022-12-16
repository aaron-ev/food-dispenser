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
#include "images.h"
#include "ili9341_touch.h"

SPI_HandleTypeDef hspi1;

#define DISPLAY_BEEP_DELAY 100
#define NO_CLEAR_ON_ENTRY 0
#define CLEAR_ALL_ON_EXIT 0xffffffffUL
#define DISPLAY_MAX_SETTINGS_OPTIONS 3
#define DISPLAY_FEED_DELAY 500
#define DISPLAY_RECT_INDICATOR_SIZE 20

/* Backlight macros*/
#define BACKLIGHT_NOT_WAIT 0
#define BACKLIGHT_TIMER_ID 1

/* Indicator coordinates */
#define DISPLAY_INDICATOR_W 21
#define DISPLAY_INDICATOR_H 15
#define DISPLAY_INDICATOR_FEED_X (52 - (DISPLAY_INDICATOR_W / 2))
#define DISPLAY_INDICATOR_FEED_Y (106 - (DISPLAY_INDICATOR_H / 2))
#define DISPLAY_INDICATOR_SETTINGS_X (52 - (DISPLAY_INDICATOR_W / 2))
#define DISPLAY_INDICATOR_SETTINGS_Y (237 - (DISPLAY_INDICATOR_H / 2))
#define DISPLAY_INDICATOR_PORTIONS_X (48 - (DISPLAY_INDICATOR_H / 2))
#define DISPLAY_INDICATOR_PORTIONS_Y (122.5 - (DISPLAY_INDICATOR_W / 2))
#define DISPLAY_INDICATOR_SOUND_X (48 - (DISPLAY_INDICATOR_H / 2))
#define DISPLAY_INDICATOR_SOUND_Y (125 - (DISPLAY_INDICATOR_W / 2))
#define DISPLAY_INDICATOR_BACK_X (153 - (DISPLAY_INDICATOR_H / 2))
#define DISPLAY_INDICATOR_BACK_Y (189 - (DISPLAY_INDICATOR_W / 2))

/*
 * Available options for the first screen.
 */
typedef enum
{
    OPTION_PORTIONS,
    OPTION_SOUND,
    OPTION_BACK,
    OPTION_FEED,
    OPTION_SETTINGS,
} Options;

typedef struct
{
    Color backgroundColor;
    uint8_t backlightState;
} DisplaySettings;

typedef enum
{
    BACKLIGHT_ON,
    BACKLIGHT_OFF,
} BackLightState;

extern DispenserSettings dispenserSettings;
DisplaySettings displaySettings;
/* freeRTOS handlers */
TaskHandle_t xTaskDisplayHandler;
TimerHandle_t xBackLightTimerHandler;

void displayShowProjectVersion(void)
{
    char buff[5];

    sprintf(buff, "v%d.%d", PROJECT_VERSION_MAYOR_NUMBER, PROJECT_VERSION_MINOR_NUMBER);
    tft_ili9341_send_str(TFT_ILI9341_WIDTH - 50, TFT_ILI9341_HEIGHT - 20, buff, Font_11x18, BLACK, WHITE);
}

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
        errorHandler();
    }
    return status;
}

void displaySetIndicator(uint8_t option)
{
    uint16_t xToSet;
    uint16_t yToSet;
    uint16_t optionClearX[3];
    uint16_t optionClearY[3];
    int i = 0;

    if (option == OPTION_FEED)
    {
        xToSet = DISPLAY_INDICATOR_FEED_X;
        yToSet = DISPLAY_INDICATOR_FEED_Y;
        optionClearX[0] = DISPLAY_INDICATOR_SETTINGS_X;
        optionClearY[0] = DISPLAY_INDICATOR_SETTINGS_Y;
        optionClearX[1] = '\0';
    }
//    if (option == OPTION_SETTINGS)
//    {
//        xToSet = DISPLAY_INDICATOR_SETTINGS_X;
//        yToSet = DISPLAY_INDICATOR_SETTINGS_Y;
//        optionClearX[0] = DISPLAY_INDICATOR_FEED_X;
//        optionClearY[0] = DISPLAY_INDICATOR_FEED_Y;
//        optionClearX[1] = '\0';
//    }
//    if (option == OPTION_PORTIONS)
//    {
//        xToSet = DISPLAY_INDICATOR_PORTIONS_X;
//        yToSet = DISPLAY_INDICATOR_PORTIONS_Y;
//        optionClearX[0] = DISPLAY_INDICATOR_SOUND_X;
//        optionClearY[0] = DISPLAY_INDICATOR_SOUND_Y;
//        optionClearX[1] = DISPLAY_INDICATOR_BACK_X;
//        optionClearY[1] = DISPLAY_INDICATOR_BACK_Y;
//        optionClearX[2] = '\0';
//    }
//    if (option == OPTION_SOUND)
//    {
//        xToSet = DISPLAY_INDICATOR_SOUND_X;
//        yToSet = DISPLAY_INDICATOR_SOUND_Y;
//        optionClearX[0] = DISPLAY_INDICATOR_PORTIONS_X;
//        optionClearY[0] = DISPLAY_INDICATOR_PORTIONS_Y;
//        optionClearX[1] = DISPLAY_INDICATOR_BACK_X;
//        optionClearY[1] = DISPLAY_INDICATOR_BACK_Y;
//        optionClearX[2] = '\0';
//    }
//    if (option == OPTION_BACK)
//    {
//        xToSet = DISPLAY_INDICATOR_BACK_X;
//        yToSet = DISPLAY_INDICATOR_BACK_Y;
//        optionClearX[0] = DISPLAY_INDICATOR_PORTIONS_X;
//        optionClearY[0] = DISPLAY_INDICATOR_PORTIONS_Y;
//        optionClearX[1] = DISPLAY_INDICATOR_SOUND_X;
//        optionClearY[1] = DISPLAY_INDICATOR_SOUND_Y;
//        optionClearX[2] = '\0';
//    }

    while (optionClearX != '\0')
    {
        tft_ili9341_fill_rectangle(optionClearX[i], optionClearY[i], DISPLAY_INDICATOR_W - 1, DISPLAY_INDICATOR_H - 1, WHITE);
        i++;
    }
    tft_ili9341_fill_rectangle(xToSet, yToSet, DISPLAY_INDICATOR_W - 1, DISPLAY_INDICATOR_H - 1, GREEN);
}

void backLightCallback(TimerHandle_t xTimer)
{
    displayBackLightOff();
}

BaseType_t displayBackLightInit(void)
{
    BaseType_t status = pdTRUE;

    /* Create one-shot timer to handle the backlight */
    xBackLightTimerHandler = xTimerCreate("backLight-timer", pdMS_TO_TICKS(DISPLAY_BACKLIGHT_DEFAULT_PERIOD), pdFALSE, (void *)BACKLIGHT_TIMER_ID, backLightCallback);
    if (xBackLightTimerHandler == NULL)
    {
        // handler no sufficient memory
        status = pdFALSE;
    }

    status = xTimerStart(xBackLightTimerHandler, BACKLIGHT_NOT_WAIT);
    if (status != pdPASS)
    {
        // TODO: Handle error
        status = pdFALSE;
    }
    displayBacklightOn();

    return status;
}

void displayShowImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* image)
{
    ILI9341_DrawImage(x, y, w, h, image);
}

void displayShowInitScreen(void)
{
    tft_ili9341_fill_screen(WHITE);
    displayShowImage(0, BACKGROUND_LEFT_H, BACKGROUND_LEFT_W, BACKGROUND_LEFT_H,
                     (const uint16_t *)background_left);
    displayShowImage(TFT_ILI9341_WIDTH - BACKGROUND_RIGHT_W, BACKGROUND_RIGHT_H,
                     BACKGROUND_RIGHT_W, BACKGROUND_RIGHT_H, (const uint16_t *)background_right);
    displayShowImage(42, 50, BUTTON_FEED_W, BUTTON_FEED_H, (const uint16_t *)button_feed);
    displayShowImage(42, 160, BUTTON_SETTINGS_W, BUTTON_SETTINGS_H, (const uint16_t *)button_settings);
//    displaySetIndicator(OPTION_FEED);
}

void displayInit(void)
{
    /* Initialize the hardware and display with default settings */
    tft_ili9341_init();
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
    uint8_t options[] = {OPTION_BACK, OPTION_SOUND, OPTION_PORTIONS};
    uint8_t minOption = options[0];
    uint8_t maxOption = options[sizeof(options) - 1];
    uint8_t index = sizeof(options) - 1; /* Max index by default */
    uint8_t optionSelected = options[index];

    // displayShowImage((const uint16_t *)settingsScreen);
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
            tft_ili9341_send_str(100, 200, buff, Font_16x26, BLACK, WHITE);
        }
        /* Option sound is selected */
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionSelected == OPTION_SOUND))
        {
            /* Toggle the sound state*/
            dispenserSettings.sound ^= 1;
            (dispenserSettings.sound == DISPENSER_SOUND_ON) ? sprintf(buff, "ON") : sprintf(buff, "OFF");
            tft_ili9341_send_str(100, 200, buff, Font_16x26, BLACK, WHITE);
        }
        /* Option back is selected */
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionSelected == OPTION_BACK))
        {
            dispenserBeep(100, 100, 1);
            displayShowInitScreen();
            break;
        }

        /* Handle button UP event */
        if (buttonEvent & BUTTON_EVENT_UP && (optionSelected != maxOption))
        {
            optionSelected = options[++index];
            displaySetIndicator(optionSelected);
        }
        if (buttonEvent & BUTTON_EVENT_UP && (optionSelected == maxOption))
        {
            dispenserBeep(50, 50, 2);
        }

        /* Handle button DOWN event */
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (optionSelected != minOption))
        {
            optionSelected = options[--index];
            displaySetIndicator(optionSelected);
        }
        if (buttonEvent & BUTTON_EVENT_DOWN && (optionSelected == minOption))
        {
            dispenserBeep(50, 50, 2);
        }

        /* Update cursor */
        displaySetIndicator(optionSelected);
    }
}

void vTaskDisplay(void *params)
{
    BaseType_t status;
    uint32_t buttonEvent;
    uint8_t cursorPosition = OPTION_FEED;

    /* Show init screen */
    displayShowInitScreen();
    displayBacklightOn();
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
            displayShowInitScreen();
            cursorPosition = OPTION_FEED;
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
            dispenserBeep(100, 100, 1);
            displaySetIndicator(OPTION_FEED);
            cursorPosition = OPTION_FEED;
        }
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (cursorPosition == OPTION_FEED))
        {
            displaySetIndicator(OPTION_SETTINGS);
            dispenserBeep(100, 100, 1);
            cursorPosition = OPTION_SETTINGS;
        }
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (cursorPosition == OPTION_SETTINGS))
        {
            dispenserBeep(50, 50, 2);
        }
    }
}
