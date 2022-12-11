
/**
  ******************************************************************************
  * @file    display.h
  * @author  Aaron Escoboza
  * @brief   Header file for the display feature
  ******************************************************************************
*/

#ifndef __DISPLAY__H
#define __DISPLAY__H
#include "FreeRTOS.h"

BaseType_t displayInit(void);
void displayWelcome(void);
#endif
