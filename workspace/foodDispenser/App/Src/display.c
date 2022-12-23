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
#include "console.h"

#define DISPLAY_BEEP_DELAY                  100
#define NO_CLEAR_ON_ENTRY                   0
#define CLEAR_ALL_ON_EXIT                   0xffffffffUL
#define DISPLAY_MAX_SETTINGS_OPTIONS        3
#define DISPLAY_FEED_DELAY                  500
#define DISPLAY_RECT_INDICATOR_SIZE         20

/* Backlight macros*/
#define BACKLIGHT_NOT_WAIT                  0
#define BACKLIGHT_TIMER_ID                  1

/* Indicator coordinates */
#define DISPLAY_INDICATOR_W                 (20 - 2)
#define DISPLAY_INDICATOR_H                 (20 - 2)

#define BEEP_DEFAULT_TON                    100
#define BEEP_DEFAULT_TOFF                   BEEP_DEFAULT_TON

#define MAX_OPTIONS                         5

/* Macros for the init screen */
#define DISPLAY_FEED_BUTTON_X               30
#define DISPLAY_FEED_BUTTON_Y               30
#define DISPLAY_SETTINGS_BUTTON_X           30
#define DISPLAY_SETTINGS_BUTTON_Y           110

/* Macros for the settings screen */
#define DISPLAY_PORTIONS_BUTTON_X           10
#define DISPLAY_PORTIONS_BUTTON_Y           25
#define DISPLAY_SOUND_BUTTON_X              10
#define DISPLAY_SOUND_BUTTON_Y              70
#define DISPLAY_BACK_BUTTON_X               65
#define DISPLAY_BACK_BUTTON_Y               220
#define DISPLAY_PORTIONS_VALUE_X            180
#define DISPLAY_PORTIONS_VALUE_Y            37
#define DISPLAY_SOUND_VALUE_X               170
#define DISPLAY_SOUND_VALUE_Y               77

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
    uint16_t x;
    uint16_t y;
} Coordinates;

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

SPI_HandleTypeDef hspi1;
Coordinates indicatorPosition[MAX_OPTIONS];
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
    PRINT_DEBUG("Backlight: ON\n");
}

void displayBackLightOff(void)
{
    HAL_GPIO_WritePin(TFT_LED_PORT, TFT_LED_PIN_NUM, GPIO_PIN_RESET);
    displaySettings.backlightState = BACKLIGHT_OFF;
    PRINT_DEBUG("Backlight: OFF\n");
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

void initIndicatorPos(void)
{
    /* Default init screen */
    indicatorPosition[OPTION_FEED].x = DISPLAY_FEED_BUTTON_X + 12;
    indicatorPosition[OPTION_FEED].y = DISPLAY_FEED_BUTTON_Y + 12;
    indicatorPosition[OPTION_SETTINGS].x = DISPLAY_SETTINGS_BUTTON_X + 12;
    indicatorPosition[OPTION_SETTINGS].y = DISPLAY_SETTINGS_BUTTON_Y + 12;
    /* Settings screen */
    indicatorPosition[OPTION_PORTIONS].x = DISPLAY_PORTIONS_BUTTON_X + 11;
    indicatorPosition[OPTION_PORTIONS].y = DISPLAY_PORTIONS_BUTTON_Y + 12;
    indicatorPosition[OPTION_SOUND].x = DISPLAY_SOUND_BUTTON_X + 12;
    indicatorPosition[OPTION_SOUND].y = DISPLAY_SOUND_BUTTON_Y + 12;
    indicatorPosition[OPTION_BACK].x = DISPLAY_BACK_BUTTON_X + 12;
    indicatorPosition[OPTION_BACK].y = DISPLAY_BACK_BUTTON_Y + 12;
}


void displaySetIndicator(Options newPos, Options oldPos)
{
    /* Clear old position */
    tft_ili9341_fill_rectangle(indicatorPosition[oldPos].x, indicatorPosition[oldPos].y,
                               DISPLAY_INDICATOR_W, DISPLAY_INDICATOR_H, WHITE);
    /* Set the new cursor */
    tft_ili9341_fill_rectangle(indicatorPosition[newPos].x, indicatorPosition[newPos].y,
                               DISPLAY_INDICATOR_W, DISPLAY_INDICATOR_H, GREEN);
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
    PRINT_DEBUG("Backlight: Initalized\n");
    // displayBacklightOn();
    return status;
}

void displayShowImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* image)
{
    ILI9341_DrawImage(x, y, w, h, image);
}

void displayBackground(void)
{
    /* Left and right background */
    displayShowImage(0, BACKGROUND_LEFT_H, BACKGROUND_LEFT_W, BACKGROUND_LEFT_H,
                     (const uint16_t *)background_left);
    displayShowImage(TFT_ILI9341_WIDTH - BACKGROUND_RIGHT_W, BACKGROUND_RIGHT_H,
                     BACKGROUND_RIGHT_W, BACKGROUND_RIGHT_H, (const uint16_t *)background_right);
}
void displayShowInitScreen(void)
{
    /* Feed and settings buttons */
    displayShowImage(DISPLAY_FEED_BUTTON_X, DISPLAY_FEED_BUTTON_Y, BUTTON_FEED_W, BUTTON_FEED_H, (const uint16_t *)button_feed);
    displayShowImage(DISPLAY_SETTINGS_BUTTON_X, DISPLAY_SETTINGS_BUTTON_Y, BUTTON_SETTINGS_W, BUTTON_SETTINGS_H, (const uint16_t *)button_settings);
    /* Set indicator to default feed option*/
    displaySetIndicator(OPTION_FEED, OPTION_SETTINGS);
    PRINT_DEBUG("Display: Default screen displayed\n");
}

void displayShowSettingsScreen(void)
{
    /* Display available options */
    displayShowImage(DISPLAY_PORTIONS_BUTTON_X, DISPLAY_PORTIONS_BUTTON_Y, BUTTON_PORTIONS_W, BUTTON_PORTIONS_H, (const uint16_t *)button_portions);
    displayShowImage(DISPLAY_SOUND_BUTTON_X, DISPLAY_SOUND_BUTTON_Y, BUTTON_SOUND_W, BUTTON_SOUND_H, (const uint16_t *)button_sound);
    displayShowImage(DISPLAY_BACK_BUTTON_X, DISPLAY_BACK_BUTTON_Y, BUTTON_BACK_W, BUTTON_BACK_H, (const uint16_t *)button_back);
    /* Set indicator to default position */
    displaySetIndicator(OPTION_PORTIONS, OPTION_SOUND);
    PRINT_DEBUG("Display: Settings screen displayed\n");
}

void displayInit(void)
{
    /* Initialize the hardware and display with default settings */
    tft_ili9341_init();
    initIndicatorPos();
}

void feed(uint8_t portions)
{
    int i;

    mspDisableButtonInterrupts();
    if ((portions == 0) || (portions > DISPENSER_MAX_PORTIONS))
    {
        dispenserBeep(BEEP_DEFAULT_TON, BEEP_DEFAULT_TOFF, 3);
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
    PRINT_DEBUG("Feed finished\n");
}

void displayPrint(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor)
{
    tft_ili9341_send_str(x, y, str, font, color, bgcolor);
}

void displayCleanScreen(void)
{
    tft_ili9341_fill_rectangle(0, 0, TFT_ILI9341_WIDTH, 155, WHITE);
    tft_ili9341_fill_rectangle(65, 0, 130, TFT_ILI9341_HEIGHT, WHITE);
}

void screenSettings(void)
{
    char buff[4];
    BaseType_t status;
    uint32_t buttonEvent;
    uint8_t options[] = {OPTION_BACK, OPTION_SOUND, OPTION_PORTIONS};
    uint8_t minOption = options[0];
    uint8_t maxOption = options[sizeof(options) - 1];
    uint8_t index = sizeof(options) - 1; /* Max index by default */
    uint8_t optionSelected = options[index];

    PRINT_DEBUG("Screen settings function\n");
    mspDisableButtonInterrupts();
    displayShowSettingsScreen();
    /* Update option values */
    sprintf(buff, "%d", dispenserSettings.portions);
    displayPrint(DISPLAY_PORTIONS_VALUE_X, DISPLAY_PORTIONS_VALUE_Y, buff, Font_16x26, BLACK,WHITE);
    (dispenserSettings.sound == DISPENSER_SOUND_ON) ? sprintf(buff, "ON") : sprintf(buff, "OFF");
    displayPrint(DISPLAY_SOUND_VALUE_X, DISPLAY_SOUND_VALUE_Y, buff, Font_16x26, BLACK,WHITE);
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
        /* Handle button ENTER event */
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionSelected == OPTION_PORTIONS))
        {
            PRINT_DEBUG("Button ENTER event. Portions option\n");
            dispenserSettings.portions++;
            if (dispenserSettings.portions > DISPENSER_MAX_PORTIONS)
            {
                dispenserSettings.portions = 1;
            }
            sprintf(buff, "%d", dispenserSettings.portions);
            /* Display portion value */
            displayPrint(DISPLAY_PORTIONS_VALUE_X, DISPLAY_PORTIONS_VALUE_Y, buff, Font_16x26, BLACK,WHITE);
        }
        /* Option sound is selected */
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionSelected == OPTION_SOUND))
        {
            PRINT_DEBUG("Button ENTER event. Sound option\n");
            /* Toggle the sound state*/
            dispenserSettings.sound ^= 1;
            (dispenserSettings.sound == DISPENSER_SOUND_ON) ? sprintf(buff, "ON") : sprintf(buff, "OFF");
            tft_ili9341_fill_rectangle(DISPLAY_SOUND_VALUE_X, DISPLAY_SOUND_VALUE_Y, 50, 26, WHITE);
            displayPrint(DISPLAY_SOUND_VALUE_X, DISPLAY_SOUND_VALUE_Y, buff, Font_16x26, BLACK,WHITE);
        }
        /* Option back is selected */
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionSelected == OPTION_BACK))
        {
            dispenserBeep(BEEP_DEFAULT_TON, BEEP_DEFAULT_TOFF, 1);
            displayCleanScreen();
            break;
        }
        /* Handle button UP event */
        if (buttonEvent & BUTTON_EVENT_UP && (optionSelected != maxOption))
        {
            optionSelected = options[++index];
            displaySetIndicator(optionSelected, options[index - 1]);
        }
        if (buttonEvent & BUTTON_EVENT_UP && (optionSelected == maxOption))
        {
            dispenserBeep(BEEP_DEFAULT_TON, BEEP_DEFAULT_TOFF, 2);
        }
        /* Handle button DOWN event */
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (optionSelected != minOption))
        {
            optionSelected = options[--index];
            displaySetIndicator(optionSelected, options[index + 1]);
        }
        if (buttonEvent & BUTTON_EVENT_DOWN && (optionSelected == minOption))
        {
            dispenserBeep(BEEP_DEFAULT_TON, BEEP_DEFAULT_TOFF, 2);
        }
    }
}

void vTaskDisplay(void *params)
{
    BaseType_t status;
    uint32_t buttonEvent;
    Options cursorIndicator = OPTION_FEED;

    tft_ili9341_fill_screen(WHITE);
    displayBackground();
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

        /* Handle button events: ENTER, UP and DOWN */
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (cursorIndicator == OPTION_FEED))
        {
            PRINT_DEBUG("Button ENTER event\n");
            dispenserBeep(BEEP_DEFAULT_TON, BEEP_DEFAULT_TOFF, 1);
            displayPrint(70, 250, "Feeding...", Font_11x18, BLACK, WHITE);
            feed(dispenserSettings.portions);
            tft_ili9341_fill_rectangle(70, 250, 105, 50, WHITE);
        }
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (cursorIndicator == OPTION_SETTINGS))
        {
            PRINT_DEBUG("Button ENTER event\n");
            dispenserBeep(BEEP_DEFAULT_TON, BEEP_DEFAULT_TOFF, 3);
            /* Clean screen */
            displayCleanScreen();
            screenSettings();
            mspEnableButtonInterrupts();
            displayShowInitScreen();
            mspEnableButtonInterrupts();
            cursorIndicator = OPTION_FEED;
        }
        if ((buttonEvent & BUTTON_EVENT_UP) && (cursorIndicator == OPTION_FEED))
        {
            PRINT_DEBUG("Button UP event\n");
            dispenserBeep(BEEP_DEFAULT_TON, BEEP_DEFAULT_TOFF, 2);
        }
        if ((buttonEvent & BUTTON_EVENT_UP) && (cursorIndicator == OPTION_SETTINGS))
        {
            PRINT_DEBUG("Button UP event \n");
            dispenserBeep(BEEP_DEFAULT_TON, BEEP_DEFAULT_TOFF, 1);
            displaySetIndicator(OPTION_FEED, OPTION_SETTINGS);
            cursorIndicator = OPTION_FEED;
        }
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (cursorIndicator == OPTION_FEED))
        {
            PRINT_DEBUG("Button DOWN event\n");
            dispenserBeep(BEEP_DEFAULT_TON, BEEP_DEFAULT_TOFF, 1);
            displaySetIndicator(OPTION_SETTINGS, OPTION_FEED);
            cursorIndicator = OPTION_SETTINGS;
        }
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (cursorIndicator == OPTION_SETTINGS))
        {
            PRINT_DEBUG("Button DOWN event\n");
            dispenserBeep(BEEP_DEFAULT_TON, BEEP_DEFAULT_TOFF, 2);
        }
    }
}
