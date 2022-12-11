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
HAL_StatusTypeDef buzzerStart(void);
HAL_StatusTypeDef buzzerStop(void);

#endif
