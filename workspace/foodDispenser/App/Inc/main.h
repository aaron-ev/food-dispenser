/**
 ******************************************************************************
 * @file         main.h
 * @author       Aaron Escoboza
 * @brief        Header to contain main definitions
 ******************************************************************************
 */

#ifndef __MAIN__H
#define __MAIN__H

#include <stdint.h>
#include <stdio.h>
#include "console.h"
#include "servoMotor.h"

/* Project version */
#define PROJECT_VERSION_MAYOR_NUMBER        0
#define PROJECT_VERSION_MINOR_NUMBER        1

#define BUTTON_INDEX_NOTIFICATION           1
#define DISPENSER_SOUND_ON                  1
#define SOUND_OFF                 0

#define PRINT_DEBUG(n)                      consolePrint(n)

typedef enum
{
    BUTTON_EVENT_UP     = (1 << 0),
    BUTTON_EVENT_DOWN   = (1 << 1),
    BUTTON_EVENT_ENTER  = (1 << 2),
    TOUCH_EVENT         = (1 << 3),
}pushButtonEvent;

typedef struct
{
    uint8_t sound; /* On or off */
    uint8_t portions;
} DispenserSettings;

void appBeep(uint32_t numTimes);
void appFeed(uint8_t portions);
void appErrorHandler(void);

#endif
