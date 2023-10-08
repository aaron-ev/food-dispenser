/**
 ******************************************************************************
 * @file         appMain.h
 * @author       Aaron Escoboza
 * @brief        Header to contain main definitions
 ******************************************************************************
 */

#ifndef __MAIN__H
#define __MAIN__H

#include <stdint.h>
#include <stdio.h>
#include "appConsole.h"
#include "bspServoMotor.h"
#include "appConfig.h"
#include "string.h"

/* Project version */
#define PROJECT_VERSION_MAJOR_NUMBER        0
#define PROJECT_VERSION_MINOR_NUMBER        1

#define BUTTON_INDEX_NOTIFICATION           1
#define DISPENSER_SOUND_ON                  1
#define SOUND_OFF                           0

#if (DEBUG_PRINT_EN == 1)
    #define PRINT_DEBUG(n)                  consolePrint(n)
#else
    #define PRINT_DEBUG(n)                  consolePrint("")
#endif
typedef enum
{
    BUTTON_EVENT_UP     = (1 << 0),
    BUTTON_EVENT_DOWN   = (1 << 1),
    BUTTON_EVENT_ENTER  = (1 << 2),
}pushButtonEvent;

typedef struct
{
    uint8_t soundState;
    uint8_t numPortions;
} DispenserSettings;

void appErrorHandler(void);
void appBeep(uint8_t numTimes);
void appFeed(uint8_t numPortions);
void appServoMotorRotate(ServoMotorDegree degree);

#endif
