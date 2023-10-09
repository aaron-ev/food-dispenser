/**
 ******************************************************************************
 * @file         display.c
 * @author       Aaron Escoboza
 * @brief        Source file for the display feature
 ******************************************************************************
 */

#include "appMain.h"
#include "appConfig.h"
#include "bsp.h"
#include "appImages.h"
#include "ili9341_touch.h"
#include "console.h"
#include "appDisplay.h"
#include "timers.h"

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


#define MAX_HOME_POS                    2
#define MAX_SETTINGS_POS                3

uint8_t homePageCoordinates[MAX_HOME_POS][2] = { 
                                        {DISP_SETTINGS_BUTTON_X + (DISP_INDICATOR_W / 2) + DISP_INDICATOR_STROKE,
                                        DISP_SETTINGS_BUTTON_Y + (DISP_INDICATOR_H / 2) + DISP_INDICATOR_STROKE},
                                        {DISP_FEED_BUTTON_X + (DISP_INDICATOR_W / 2) + DISP_INDICATOR_STROKE, 
                                        DISP_FEED_BUTTON_Y + (DISP_INDICATOR_H / 2) + DISP_INDICATOR_STROKE }
                                    };
uint8_t settingsPageCoordinates[MAX_SETTINGS_POS][2] = {
                                            {DISP_BACK_BUTTON_X + (DISP_INDICATOR_W / 2) + DISP_INDICATOR_STROKE,
                                            DISP_BACK_BUTTON_Y + (DISP_INDICATOR_H / 2) + DISP_INDICATOR_STROKE},
                                            {DISP_SOUND_BUTTON_X + (DISP_INDICATOR_W / 2) + DISP_INDICATOR_STROKE,
                                            DISP_SOUND_BUTTON_Y + (DISP_INDICATOR_H / 2) + DISP_INDICATOR_STROKE},
                                            {DISP_PORTIONS_BUTTON_X + (DISP_INDICATOR_W / 2) + DISP_INDICATOR_STROKE,
                                            DISP_PORTIONS_BUTTON_Y + (DISP_INDICATOR_H / 2) + DISP_INDICATOR_STROKE}
                                        };
typedef enum 
{
    HOME_STATE,
    SETTINGS_STATE,
}DisplayStates;

typedef enum 
{
    FEED_EVENT,
    SETTINGS_EVENT,
    SOUND_EVENT,
    PORTIONS_EVENT,
    BACK_EVENT,
}DisplayEvents;

typedef enum
{
    OPTION_BACK,
    OPTION_SOUND,
    OPTION_PORTIONS,
    MAX_SETTINGS_PAGE_OPS,
} SettingsPageOptions;

typedef enum
{
    OPTION_SETTINGS,
    OPTION_FEED,
    MAX_HOME_PAGE_OPS,
} HomePageOptions;

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

/* Helper functions to handle the project version */
static void dispShowVersion(void);

/* Helper functions to handle the option indicator */
//static void displayInitOptIndicator(void);
static void dispSetOpIndicator(uint8_t newPos, DisplayStates state);

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

    sprintf(buff, "v%d.%d", PROJECT_VERSION_MAJOR_NUMBER, PROJECT_VERSION_MINOR_NUMBER);
    tft_ili9341_send_str(TFT_ILI9341_WIDTH - 50, 20,
                         buff, Font_11x18, BLACK, WHITE);
}

/*
 * Turn the backlight on.
 */
static void dispSetBacklightOn(void)
{
    HAL_GPIO_WritePin(TFT_LED_PORT, TFT_LED_PIN_NUM, GPIO_PIN_SET);
    backlightState = BACKLIGHT_ON;
    PRINT_DEBUG("DEBUG: Backlight ON\n");
}

/*
 * Turn the backlight off.
 */
static void dispSetBackLightOff(void)
{
    HAL_GPIO_WritePin(TFT_LED_PORT, TFT_LED_PIN_NUM, GPIO_PIN_RESET);
    backlightState = BACKLIGHT_OFF;
    PRINT_DEBUG("DEBUG: Backlight OFF\n");
}

/*
 * Reset the backlight timer.
 */
BaseType_t dispBacklightTimReset(void)
{
    return xTimerReset(xBackLightTimerHandler, BACKLIGHT_NOT_WAIT);
}

/*
 * Set the option indicator in a new position.
 */
static void dispSetOpIndicator(uint8_t newPos, DisplayStates state)
{
    int i;

    if (state == HOME_STATE)
    {
        /* Set the new cursor */
        tft_ili9341_fill_rectangle(homePageCoordinates[newPos][0], homePageCoordinates[newPos][1],
                                DISP_INDICATOR_W - DISP_INDICATOR_STROKE, DISP_INDICATOR_H - DISP_INDICATOR_STROKE, GREEN);
        /* Clear old position */
        for(i = 0; i < MAX_HOME_POS; i++)
        {
            if (i != newPos)
            {
                tft_ili9341_fill_rectangle(homePageCoordinates[i][0], homePageCoordinates[i][1],
                            DISP_INDICATOR_W - DISP_INDICATOR_STROKE, DISP_INDICATOR_H - DISP_INDICATOR_STROKE, WHITE);
            }
        }
    }
    else if (state == SETTINGS_STATE)
    {
        /* Set the new cursor */
        tft_ili9341_fill_rectangle(settingsPageCoordinates[newPos][0], settingsPageCoordinates[newPos][1],
                                DISP_INDICATOR_W - DISP_INDICATOR_STROKE, DISP_INDICATOR_H - DISP_INDICATOR_STROKE, GREEN);
        /* Clear old position */
        for(i = 0; i < MAX_SETTINGS_POS; i++)
        {
            if (i != newPos)
            {
                tft_ili9341_fill_rectangle(settingsPageCoordinates[i][0], settingsPageCoordinates[i][1],
                            DISP_INDICATOR_W - DISP_INDICATOR_STROKE, DISP_INDICATOR_H - DISP_INDICATOR_STROKE, WHITE);
            }
        }
    }
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
    xBackLightTimerHandler = xTimerCreate("backLight-timer", pdMS_TO_TICKS(DISP_BACKLIGHT_DEFAULT_PERIOD),
                                          pdFALSE, (void *)BACKLIGHT_TIMER_ID, dispBackLightCallback);
    if (xBackLightTimerHandler == NULL)
    {
        return pdFALSE;
    }
    PRINT_DEBUG("DEBUG: backlight: Timer created\n");
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
    mspDisableButtonIT();
    /* Feed and settings buttons */
    dispShowImage(DISP_FEED_BUTTON_X, DISP_FEED_BUTTON_Y,
                  BUTTON_FEED_W, BUTTON_FEED_H, (const uint16_t *)button_feed);
    dispShowImage(DISP_SETTINGS_BUTTON_X, DISP_SETTINGS_BUTTON_Y,
                  BUTTON_SETTINGS_W, BUTTON_SETTINGS_H, (const uint16_t *)button_settings);
    mspEnableButtonIT();
    dispShowVersion();
    PRINT_DEBUG("DEBUG: Default screen displayed\n");
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
    PRINT_DEBUG("DEBUG: Settings screen displayed\n");
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
    dispFillScreen(WHITE);
    dispShowWallPaper();
    dispShowInitScreen();
}

void appDisplayWaitButtonEvent(pushButtonEvent *buttonEvent)
{
    /* Wait until a push button is pressed.
     * Notification settings: Index > 0, no clear on entry,
     *                       Clear all events on exit,
     *                       Block until there is an event.
     */
    xTaskNotifyWaitIndexed(BUTTON_INDEX_NOTIFICATION, NO_CLEAR_ON_ENTRY, CLEAR_ALL_ON_EXIT, buttonEvent, portMAX_DELAY);
}

DisplayStates appDisplayHandleSettingsPage(SettingsPageOptions optionSelected)
{
    char buff[15];
    DisplayStates nextState = SETTINGS_STATE;

    switch (optionSelected)
    {
        case OPTION_PORTIONS:
        {
            // appBeep(DISP_BEEP_ONCE);
            dispenserSettings.numPortions++;
            if (dispenserSettings.numPortions > DISPENSER_MAX_PORTIONS)
            {
                dispenserSettings.numPortions = 1;
            }
            sprintf(buff, "%d", dispenserSettings.numPortions);
            /* Display portion value */
            dispPrint(DISP_PORTIONS_VALUE_X, DISP_PORTIONS_VALUE_Y, buff, Font_16x26, BLACK,WHITE);
            consolePrint("APP: Portions: ");
            consolePrint(buff);
            consolePrint("\n");
        }
        break;
        case OPTION_SOUND:
        {
            /* Toggle sound state*/
            dispenserSettings.soundState ^= 1;
            if (dispenserSettings.soundState)
            {
                consolePrint("APP: Sound ON\n");
                sprintf(buff, "ON");
            }
            else
            {
                consolePrint("APP: Sound OFF\n");
                sprintf(buff, "OFF");
            }
            dispFillRect(DISP_SOUND_VALUE_X, DISP_SOUND_VALUE_Y, 50, 26, WHITE);
            dispPrint(DISP_SOUND_VALUE_X, DISP_SOUND_VALUE_Y, buff, Font_16x26, BLACK,WHITE);
        }
        break;
        case OPTION_BACK: nextState = HOME_STATE;
        break;
    default:
        break;
    }

    return nextState;
}

/*
 * Task to handle the display.
 */
void vTaskDisplay(void *params)
{
    char buff[15]; 
    BaseType_t status;
    pushButtonEvent buttonEvent;
    DisplayStates nextState = HOME_STATE;
    HomePageOptions homePageOption = OPTION_FEED;
    SettingsPageOptions settingPageOption = OPTION_PORTIONS;

    /* Initialize the backlight timer */
    status = dispBacklightTimInit();
    if (status != pdTRUE)
    {
        consolePrint("APP: Backlight timer could not be initialize\n");
        appErrorHandler();
    }
    status = dispBacklightTimStart();
    if (status != pdTRUE)
    {
        consolePrint("APP: Backlight timer could not be started\n");
        appErrorHandler();
    }

    /* Enable Buttons: Interrupts after first screen is ready */
    mspEnableButtonIT();
    appServoMotorRotate(SERVOMOTOR_DEGREES_0);
    dispShowInitScreen();
    dispSetOpIndicator(OPTION_FEED, nextState);
    dispSetBacklightOn();

    while (1)
    {
        appDisplayWaitButtonEvent(&buttonEvent);
        appBeep(DISP_BEEP_ONCE);

        if ((buttonEvent & BUTTON_EVENT_UP) && (nextState == HOME_STATE))
        {
            if (homePageOption < MAX_HOME_PAGE_OPS - 1)
                homePageOption++;
        }
        if ((buttonEvent & BUTTON_EVENT_UP) && (nextState == SETTINGS_STATE))
        {
            if (settingPageOption < MAX_SETTINGS_PAGE_OPS - 1)
                settingPageOption++;
        }
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (nextState == HOME_STATE))
        {
            if (homePageOption > 0)
                homePageOption--;
        }
        if ((buttonEvent & BUTTON_EVENT_DOWN) && (nextState == SETTINGS_STATE))
        {
            if (settingPageOption > 0 )
                settingPageOption--;
        }

        /* Reset the backlight timer every time there is button event */
        status = dispBacklightTimReset();
        if (status != pdTRUE)
        {
            PRINT_DEBUG("DEBUG: Backlight timer could not be restarted\n");
        }
        if (backlightState == BACKLIGHT_OFF)
        {
            /* Coming from a backlight off a push button event does not cause any action
               so it just continues until another button event.
            */
            dispSetBacklightOn();
            continue;
        }

        switch (nextState)
        {
            case HOME_STATE:
            {
                dispSetOpIndicator(homePageOption, HOME_STATE);
                if (buttonEvent != BUTTON_EVENT_ENTER)
                    break;
                
                if (homePageOption == OPTION_FEED)
                {
                    appFeed(dispenserSettings.numPortions);
                }
                else if (homePageOption == OPTION_SETTINGS)
                {
                    mspDisableButtonIT();
                    dispCleanScreen();
                    dispShowSettingsScreen();
                    sprintf(buff, "%d", dispenserSettings.numPortions);
                    dispPrint(DISP_PORTIONS_VALUE_X, DISP_PORTIONS_VALUE_Y, buff, Font_16x26, BLACK, WHITE);
                    (dispenserSettings.soundState == DISPENSER_SOUND_ON) ? sprintf(buff, "ON") : sprintf(buff, "OFF");
                    dispPrint(DISP_SOUND_VALUE_X, DISP_SOUND_VALUE_Y, buff, Font_16x26, BLACK,WHITE);
                    nextState = SETTINGS_STATE;
                    dispSetOpIndicator(OPTION_PORTIONS, nextState);
                    mspEnableButtonIT();
                }
                break;
            } 
            case SETTINGS_STATE:
            {
                dispSetOpIndicator(settingPageOption, nextState);
                if (buttonEvent != BUTTON_EVENT_ENTER)
                    break;
                nextState = appDisplayHandleSettingsPage(settingPageOption);
                if (nextState == HOME_STATE)
                {
                    dispCleanScreen();
                    dispShowInitScreen();
                	dispSetOpIndicator(OPTION_FEED, nextState);
                	homePageOption = OPTION_FEED;
                	settingPageOption = OPTION_PORTIONS;
                }
                break;
            }
        default:
            break;
        }
    }
}
