/**
 ******************************************************************************
 * @file         main.h
 * @author       Aaron Escoboza
 * @brief        Header to contain main definitions
 ******************************************************************************
 */

#ifndef __MAIN__H
#define __MAIN_H

#include <stdint.h>
#include <stdio.h>

#define BUTTON_INDEX_NOTIFICATION           1
#define DISPENSER_SOUND_ON                  1
#define DISPENSER_SOUND_OFF                 0

typedef enum
{
    BUTTON_EVENT_UP     = (1 << 0),
    BUTTON_EVENT_DOWN   = (1 << 1),
    BUTTON_EVENT_ENTER  = (1 << 2),
}pushButtonEvent;

typedef struct
{
    uint8_t sound; /* On or off */
    uint8_t portions;
}DispenserSettings;

void dispenserBeep(uint32_t timeOn, uint32_t timeOff, uint32_t times);

#endif
