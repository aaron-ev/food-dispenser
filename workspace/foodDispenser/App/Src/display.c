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
#include "display.h"

/* FreeRTOS helper macros */
#define NO_CLEAR_ON_ENTRY                0
#define CLEAR_ALL_ON_EXIT                0xffffffffUL

/* Backlight macros*/
#define BACKLIGHT_NOT_WAIT               0
#define BACKLIGHT_TIMER_ID               1

/* Indicator coordinates */
#define DISP_INDICATOR_STROKE            2
#define DISP_INDICATOR_W                 20
#define DISP_INDICATOR_H                 20

/* Sum of options in the init and settings screen */
#define MAX_OPTIONS                      5

/* Macros for the init screen */
#define DISP_FEED_BUTTON_X               30
#define DISP_FEED_BUTTON_Y               60
#define DISP_SETTINGS_BUTTON_X           30
#define DISP_SETTINGS_BUTTON_Y           150

/* Portion position on the settings screen */
#define DISP_PORTIONS_BUTTON_X           10
#define DISP_PORTIONS_BUTTON_Y           25
#define DISP_PORTIONS_VALUE_X            180
#define DISP_PORTIONS_VALUE_Y            37

/* Sound position on the settings screen */
#define DISP_SOUND_BUTTON_X              10
#define DISP_SOUND_BUTTON_Y              70
#define DISP_SOUND_VALUE_X               170
#define DISP_SOUND_VALUE_Y               77

/* Back position on the settings screen */
#define DISP_BACK_BUTTON_X               65
#define DISP_BACK_BUTTON_Y               160

/* Helper macros to handle the beep */
#define DISP_BEEP_ONCE                   1

/*
 * Available options for all screens.
 */
typedef enum
{
    OPTION_PORTIONS,
    OPTION_SOUND,
    OPTION_BACK,
    OPTION_FEED,
    OPTION_SETTINGS,
} Options;

/*
 * Coordinates to display images and shapes on the screen.
 */
typedef struct
{
    uint16_t x;
    uint16_t y;
} Coordinates;

/*
 * Backlight possible states.
 */
typedef enum
{
    BACKLIGHT_ON,
    BACKLIGHT_OFF,
} BackLightState;

/* Helper global variables */
Coordinates indicatorPosition[MAX_OPTIONS];
BackLightState backlightState;
extern DispenserSettings dispenserSettings;

/* freeRTOS handlers */
TaskHandle_t xTaskDisplayHandler;
TimerHandle_t xBackLightTimerHandler;

/* Helper function to initialize the display */
void displayInit(void);

/* Helper functions to handle the project version */
static void dispShowVersion(void);

/* Helper functions to handle the option indicator */
static void displayInitOptIndicator(void);
static void dispSetOpIndicator(Options newPos, Options oldPos);

/* Backlight helper functions */
static void dispSetBackLightOff(void);
static void dispSetBacklightOn(void);
static void dispBackLightCallback(TimerHandle_t xTimer);
BaseType_t dispBacklightTimInit(void);
BaseType_t dispBacklightTimStart(void);
BaseType_t dispBacklightTimReset(void);

/* Display helper functions */
static void dispShowImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* image);
static void dispShowWallPaper(void);
static void dispShowInitScreen(void);
static void dispShowSettingsScreen(void);
static void dispCleanScreen(void);
static void dispFillScreen(Color color);

/*
 * Show the project version on the screen.
 */
static void dispShowVersion(void)
{
    char buff[5];

    sprintf(buff, "v%d.%d", PROJECT_VERSION_MAYOR_NUMBER, PROJECT_VERSION_MINOR_NUMBER);
    tft_ili9341_send_str(TFT_ILI9341_WIDTH - 50, TFT_ILI9341_HEIGHT - 20, buff, Font_11x18, BLACK, WHITE);
}

/*
 * Turn the backlight on.
 */
static void dispSetBacklightOn(void)
{
    HAL_GPIO_WritePin(TFT_LED_PORT, TFT_LED_PIN_NUM, GPIO_PIN_SET);
    backlightState = BACKLIGHT_ON;
    PRINT_DEBUG("Backlight: ON\n");
}

/*
 * Turn the backlight off.
 */
static void dispSetBackLightOff(void)
{
    HAL_GPIO_WritePin(TFT_LED_PORT, TFT_LED_PIN_NUM, GPIO_PIN_RESET);
    backlightState = BACKLIGHT_OFF;
    PRINT_DEBUG("Backlight: OFF\n");
}

/*
 * Reset the backlight timer.
 */
BaseType_t dispBacklightTimReset(void)
{
    return xTimerReset(xBackLightTimerHandler, BACKLIGHT_NOT_WAIT);
}

/*
 * Display the option indicator on the screen.
 */
static void displayInitOptIndicator(void)
{
    /* Default init screen */
    indicatorPosition[OPTION_FEED].x = DISP_FEED_BUTTON_X + (DISP_INDICATOR_W / 2) + DISP_INDICATOR_STROKE;
    indicatorPosition[OPTION_FEED].y = DISP_FEED_BUTTON_Y + (DISP_INDICATOR_H / 2) + DISP_INDICATOR_STROKE;
    indicatorPosition[OPTION_SETTINGS].x = DISP_SETTINGS_BUTTON_X + (DISP_INDICATOR_W / 2) + DISP_INDICATOR_STROKE;
    indicatorPosition[OPTION_SETTINGS].y = DISP_SETTINGS_BUTTON_Y + (DISP_INDICATOR_H / 2) + DISP_INDICATOR_STROKE;
    /* Settings screen */
    indicatorPosition[OPTION_PORTIONS].x = DISP_PORTIONS_BUTTON_X + (DISP_INDICATOR_W / 2) + DISP_INDICATOR_STROKE;
    indicatorPosition[OPTION_PORTIONS].y = DISP_PORTIONS_BUTTON_Y + (DISP_INDICATOR_H / 2) + DISP_INDICATOR_STROKE;
    indicatorPosition[OPTION_SOUND].x = DISP_SOUND_BUTTON_X + (DISP_INDICATOR_W / 2) + DISP_INDICATOR_STROKE;
    indicatorPosition[OPTION_SOUND].y = DISP_SOUND_BUTTON_Y + (DISP_INDICATOR_H / 2) + DISP_INDICATOR_STROKE;
    indicatorPosition[OPTION_BACK].x = DISP_BACK_BUTTON_X + (DISP_INDICATOR_W / 2) + DISP_INDICATOR_STROKE;
    indicatorPosition[OPTION_BACK].y = DISP_BACK_BUTTON_Y + (DISP_INDICATOR_H / 2) + DISP_INDICATOR_STROKE;
}

/*
 * Set the option indicator in a new position.
 */
static void dispSetOpIndicator(Options newPos, Options oldPos)
{
    /* Clear old position */
    tft_ili9341_fill_rectangle(indicatorPosition[oldPos].x, indicatorPosition[oldPos].y,
                               DISP_INDICATOR_W - DISP_INDICATOR_STROKE, DISP_INDICATOR_H - DISP_INDICATOR_STROKE, WHITE);
    /* Set the new cursor */
    tft_ili9341_fill_rectangle(indicatorPosition[newPos].x, indicatorPosition[newPos].y,
                               DISP_INDICATOR_W - DISP_INDICATOR_STROKE, DISP_INDICATOR_H - DISP_INDICATOR_STROKE, GREEN);
}

/*
 * Callback for the backlight timer.
 */
static void dispBackLightCallback(TimerHandle_t xTimer)
{
    dispSetBackLightOff();
}

/*
 * Start the backlight timer.
 */
BaseType_t dispBacklightTimStart(void)
{
    return xTimerStart(xBackLightTimerHandler, BACKLIGHT_NOT_WAIT);
}

/*
 * Initialize the backlight timer.
 */
BaseType_t dispBacklightTimInit(void)
{
    /* Create one-shot timer to handle the backlight */
    xBackLightTimerHandler = xTimerCreate("backLight-timer", pdMS_TO_TICKS(DISP_BACKLIGHT_DEFAULT_PERIOD), pdFALSE, (void *)BACKLIGHT_TIMER_ID, dispBackLightCallback);
    if (xBackLightTimerHandler == NULL)
    {
        return pdFALSE;
    }
    PRINT_DEBUG("Backlight: Timer created\n");
    return pdTRUE;
}

/*
 * Display an image on the screen.
 */
static void dispShowImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* image)
{
    ILI9341_DrawImage(x, y, w, h, image);
}

/*
 * Display the wallpaper on the screen.
 */
static void dispShowWallPaper(void)
{
    dispShowImage(0, TFT_ILI9341_HEIGHT - BACKGROUND_CAT_H,
                    BACKGROUND_CAT_W, BACKGROUND_CAT_H, (const uint16_t *)background_cat);
}

/*
 * Display the init screen.
 */
static void dispShowInitScreen(void)
{
    mspDisableButtonInterrupts();
    /* Feed and settings buttons */
    dispShowImage(DISP_FEED_BUTTON_X, DISP_FEED_BUTTON_Y,
                    BUTTON_FEED_W, BUTTON_FEED_H, (const uint16_t *)button_feed);
    dispShowImage(DISP_SETTINGS_BUTTON_X, DISP_SETTINGS_BUTTON_Y,
                    BUTTON_SETTINGS_W, BUTTON_SETTINGS_H, (const uint16_t *)button_settings);
    /* Set indicator to default feed option*/
    dispSetOpIndicator(OPTION_FEED, OPTION_SETTINGS);
    mspEnableButtonInterrupts();
    PRINT_DEBUG("Display: Default screen displayed\n");
}

/*
 * Display the settings screen.
 */
static void dispShowSettingsScreen(void)
{
    /* Display available options */
    dispShowImage(DISP_PORTIONS_BUTTON_X, DISP_PORTIONS_BUTTON_Y,
                    BUTTON_PORTIONS_W, BUTTON_PORTIONS_H, (const uint16_t *)button_portions);
    dispShowImage(DISP_SOUND_BUTTON_X, DISP_SOUND_BUTTON_Y,
                    BUTTON_SOUND_W, BUTTON_SOUND_H, (const uint16_t *)button_sound);
    dispShowImage(DISP_BACK_BUTTON_X, DISP_BACK_BUTTON_Y,
                    BUTTON_BACK_W, BUTTON_BACK_H, (const uint16_t *)button_back);
    /* Set indicator to default position */
    dispSetOpIndicator(OPTION_PORTIONS, OPTION_SOUND);
    PRINT_DEBUG("Display: Settings screen displayed\n");
}

/*
 * Print a message on the screen.
 */
void dispPrint(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor)
{
    tft_ili9341_send_str(x, y, str, font, color, bgcolor);
}

/*
 * Clean the screen.
 */
static void dispCleanScreen(void)
{
    tft_ili9341_fill_rectangle(0, 0, TFT_ILI9341_WIDTH, TFT_ILI9341_HEIGHT - BACKGROUND_CAT_H, WHITE);
}

/*
 * Display a rectangle and fills it.
 */
void dispFillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    tft_ili9341_fill_rectangle(x, y, w, h, color);
}

/*
 * Fill the whole screen.
 */
static void dispFillScreen(Color color)
{
    tft_ili9341_fill_screen(color);
}

/*
 * Initialize the display.
 */
void displayInit(void)
{
    /* Initialize the hardware and display with default init screen */
    tft_ili9341_init();
    displayInitOptIndicator();
    dispFillScreen(WHITE);
    dispShowWallPaper();
    dispShowInitScreen();
}

/*
 * Handles the settings screen when enter on that option.
 */
static void dispScreenSettings(void)
{
    char buff[4];
    BaseType_t status;
    uint32_t buttonEvent;
    uint8_t options[] = {OPTION_BACK, OPTION_SOUND, OPTION_PORTIONS};
    uint8_t minOption = options[0];
    uint8_t maxOption = options[sizeof(options) - 1];
    uint8_t index = sizeof(options) - 1; /* Max index by default */
    uint8_t optionSelected = options[index];

    mspDisableButtonInterrupts();
    dispShowSettingsScreen();
    /* Update option values */
    sprintf(buff, "%d", dispenserSettings.portions);
    dispPrint(DISP_PORTIONS_VALUE_X, DISP_PORTIONS_VALUE_Y, buff, Font_16x26, BLACK,WHITE);
    (dispenserSettings.sound == DISPENSER_SOUND_ON) ? sprintf(buff, "ON") : sprintf(buff, "OFF");
    dispPrint(DISP_SOUND_VALUE_X, DISP_SOUND_VALUE_Y, buff, Font_16x26, BLACK,WHITE);
    mspEnableButtonInterrupts();
    while (1)
    {
        /* Wait until a push button is pressed.
         * Notification settings: Index > 0, no clear on entry,
         *                       Clear all events on exit,
         *                       Block until there is an event.
         */
        xTaskNotifyWaitIndexed(BUTTON_INDEX_NOTIFICATION, NO_CLEAR_ON_ENTRY, CLEAR_ALL_ON_EXIT, &buttonEvent, portMAX_DELAY);
        status = dispBacklightTimReset();
        if (status != pdTRUE)
        {
            appErrorHandler();
        }
        if (backlightState == BACKLIGHT_OFF)
        {
            dispSetBacklightOn();
            continue;
        }
        /* Handle Buttons: ENTER event */
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionSelected == OPTION_PORTIONS))
        {
            appBeep(DISP_BEEP_ONCE);
            dispenserSettings.portions++;
            if (dispenserSettings.portions > DISPENSER_MAX_PORTIONS)
            {
                dispenserSettings.portions = 1;
            }
            sprintf(buff, "%d", dispenserSettings.portions);
            /* Display portion value */
            dispPrint(DISP_PORTIONS_VALUE_X, DISP_PORTIONS_VALUE_Y, buff, Font_16x26, BLACK,WHITE);
            PRINT_DEBUG("Buttons: ENTER event. Portions:");
            PRINT_DEBUG(buff);
            PRINT_DEBUG("\n");
        }
        /* Option sound is selected */
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionSelected == OPTION_SOUND))
        {
            PRINT_DEBUG("Buttons: ENTER event. Sound option\n");
            appBeep(DISP_BEEP_ONCE);
            /* Toggle sound state*/
            dispenserSettings.sound ^= 1;
            (dispenserSettings.sound == DISPENSER_SOUND_ON) ? sprintf(buff, "ON") : sprintf(buff, "OFF");
            dispFillRect(DISP_SOUND_VALUE_X, DISP_SOUND_VALUE_Y, 50, 26, WHITE);
            dispPrint(DISP_SOUND_VALUE_X, DISP_SOUND_VALUE_Y, buff, Font_16x26, BLACK,WHITE);
        }
        /* Option back is selected */
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionSelected == OPTION_BACK))
        {
            appBeep(DISP_BEEP_ONCE);
            break;
        }
        /* Handle Buttons: UP event */
        if (buttonEvent & BUTTON_EVENT_UP && (optionSelected != maxOption))
        {
            optionSelected = options[++index];
            dispSetOpIndicator(optionSelected, options[index - 1]);
            appBeep(DISP_BEEP_ONCE);
        }
        /* Handle Buttons: DOWN event */
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (optionSelected != minOption))
        {
            optionSelected = options[--index];
            dispSetOpIndicator(optionSelected, options[index + 1]);
            appBeep(DISP_BEEP_ONCE);
        }
    }
}

/*
 * Task to handle the display.
 */
void vTaskDisplay(void *params)
{
    BaseType_t status;
    uint32_t buttonEvent;
    Options optionIndicator = OPTION_FEED;

    dispSetBacklightOn();
    /* Initialize the backlight timer */
    status = dispBacklightTimInit();
    if (status != pdTRUE)
    {
        appErrorHandler();
    }
    status = dispBacklightTimStart();
    if (status != pdTRUE)
    {
        appErrorHandler();
    }

    /* Enable Buttons: Interrupts after first screen is ready */
    mspEnableButtonInterrupts();
    while (1)
    {
        /* Wait until a push button is pressed.
         * Notification settings: Index > 0, no clear on entry,
         *                       Clear all events on exit,
         *                       Block until there is an event.
         */
        xTaskNotifyWaitIndexed(BUTTON_INDEX_NOTIFICATION, NO_CLEAR_ON_ENTRY, CLEAR_ALL_ON_EXIT, &buttonEvent, portMAX_DELAY);
        /* Reset the backlight timer every time there is button event */
        status = dispBacklightTimReset();
        if (status != pdTRUE)
        {
            appErrorHandler();
        }
        if (backlightState == BACKLIGHT_OFF)
        {
            /* Coming from a backlight off a push button event does not cause any action
               so it just continues until another button event.
            */
            dispSetBacklightOn();
            continue;
        }

        /* Handle button events: ENTER, UP and DOWN */
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionIndicator == OPTION_FEED))
        {
            PRINT_DEBUG("Buttons: ENTER event\n");
            appBeep(DISP_BEEP_ONCE);
            appFeed(dispenserSettings.portions);
        }
        if ((buttonEvent & BUTTON_EVENT_ENTER) && (optionIndicator == OPTION_SETTINGS))
        {
            PRINT_DEBUG("Buttons: ENTER event\n");
            appBeep(DISP_BEEP_ONCE);
            /* Clean the push buttons icons and only keep the wallpaper before
            *  displaying a screen.
            */
            dispCleanScreen();
            dispScreenSettings();
            dispCleanScreen();
            dispShowInitScreen();
            optionIndicator = OPTION_FEED;
        }
        if ((buttonEvent & BUTTON_EVENT_UP) && (optionIndicator == OPTION_SETTINGS))
        {
            PRINT_DEBUG("Buttons: UP event \n");
            appBeep(DISP_BEEP_ONCE);
            dispSetOpIndicator(OPTION_FEED, OPTION_SETTINGS);
            optionIndicator = OPTION_FEED;
        }
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (optionIndicator == OPTION_FEED))
        {
            PRINT_DEBUG("Buttons: DOWN event\n");
            appBeep(DISP_BEEP_ONCE);
            dispSetOpIndicator(OPTION_SETTINGS, OPTION_FEED);
            optionIndicator = OPTION_SETTINGS;
        }
    }
}
