/**
  ******************************************************************************
  * @file    buzzer.h
  * @author  Aaron Escoboza
  * @brief   Header file to manage the a buzzer.
  ******************************************************************************
*/

#ifndef __BUZZER__H
#define __BUZZER_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "appConfig.h"
#include "stdint.h"

HAL_StatusTypeDef buzzerInit(void);
void buzzerBeep(uint32_t timeOn, uint32_t timeOff, uint32_t times);

#endif
