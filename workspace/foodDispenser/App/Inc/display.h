
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
#include "fonts.h"

void dispInit(void);
void dispPrint(uint16_t x, uint16_t y, const char* str,
               FontDef font, uint16_t color, uint16_t bgcolor);
void dispFillRect(uint16_t x, uint16_t y, uint16_t w,
                  uint16_t h, uint16_t color);
#endif
