/**
  ******************************************************************************
  * @file    buzzer.h
  * @author  Aaron Escoboza
  * @brief   Header file to manage the a buzzer.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#ifndef __BUZZER__H
#define __BUZZER_H
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

HAL_StatusTypeDef buzzerInit(uint32_t buzzerTimPeriod, uint32_t pin, GPIO_TypeDef *GPIOx);
HAL_StatusTypeDef buzzerStart(void);
void buzzerStop(void);

#endif
