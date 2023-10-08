/**
  ******************************************************************************
  * @file    bsp.h
  * @author  Aaron Escoboza
  * @brief   Header file to expose init functions
  ******************************************************************************
*/


#ifndef __BSP__H
#define __BSP__H

#include "appConfig.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "bspServoMotor.h"
#include "bspBuzzer.h"
#include "appMain.h"
#include "msp.h"
#include "tft_ili9341.h"

HAL_StatusTypeDef bspInit(void);
// void (*bspErrorCallback)(void);
// void bspSetErrorCallback(void *callback);
#endif
