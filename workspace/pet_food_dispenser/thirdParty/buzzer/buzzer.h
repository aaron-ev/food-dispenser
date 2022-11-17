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


#define BUZZER_GPIO_INSTANCE          GPIOA
#define BUZZER_GPIO_PIN_NUM           GPIO_PIN_0
#define BUZZER_GPIO_ALTERNATE         GPIO_AF1_LPTIM
#define BUZZER_TIMER_INSTANCE         TIM2
#define BUZZER_TIMER_CHANNEL          TIM_CHANNEL_1
#define BUZZER_FREQUENCY              1000 /* Khz*/

HAL_StatusTypeDef buzzerInit(void);
HAL_StatusTypeDef buzzerEnable(void);
HAL_StatusTypeDef buzzerDisable(void);

#endif
