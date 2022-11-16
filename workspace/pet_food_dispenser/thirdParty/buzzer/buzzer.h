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

typedef enum
{
    BUZZER_DISABLE,
    BUZZER_ENABLE
}BuzzerInitalState;

typedef struct
{
    uint32_t freq; /* Buzzer frequency */
    BuzzerInitalState buzzerState;
    TIM_HandleTypeDef timerHandler;
    uint8_t channel;
}Buzzer;

void buzzerInit(Buzzer *buzzer);
void buzzerEnable(Buzzer *buzzer);
void buzzerDisable(Buzzer *buzzer);

#endif