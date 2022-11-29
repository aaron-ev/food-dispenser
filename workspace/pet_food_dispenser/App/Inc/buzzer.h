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
#include "stm32f4xx_hal.h"

HAL_StatusTypeDef buzzerInit(GPIO_TypeDef *GPIOx, uint32_t pin, TIM_TypeDef *sTIMx, uint32_t channel);
HAL_StatusTypeDef buzzerStart(uint32_t channel);
HAL_StatusTypeDef buzzerStop(uint32_t channel);

#endif
