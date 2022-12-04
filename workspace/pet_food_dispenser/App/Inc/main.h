/**
 ******************************************************************************
 * @file         main.h
 * @author       Aaron Escoboza
 * @brief        Header to contain main definitions
 ******************************************************************************
 */

#ifndef __MAIN__H
#define __MAIN_H

typedef enum
{
    BUTTON_EVENT_UP     = (1 << 0),
    BUTTON_EVENT_DOWN   = (1 << 1),
    BUTTON_EVENT_ENTER  = (1 << 2),
}pushButtonEvent;

#endif