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
#include "console.h"
#include "servoMotor.h"
#include "buzzer.h"
#include "main.h"
#include "msp.h"
#include "tft_ili9341.h"
#include "ds1302/ds1302.h"


HAL_StatusTypeDef bspInit(void);

#endif
