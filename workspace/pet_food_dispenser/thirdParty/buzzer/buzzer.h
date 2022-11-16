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

#define BUZZER_TIM_INSTANCE  TIM4


#define BUZZER_GPIO_INSTANCE
#define BUZZER_GPIO_PIN_NUM
#define BUZZER_GPIO_ALTERNATE
#define BUZZER_TIMER_INSTANCE           TIM4
#define BUZZER_TIMER_CHANNEL			   1
#define BUZZER_FREQUENCY

HAL_StatusTypeDef buzzerInit(void);
HAL_StatusTypeDef buzzerEnable(void);
HAL_StatusTypeDef buzzerDisable(void);

#endif
