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

HAL_StatusTypeDef buzzerInit(void);
HAL_StatusTypeDef buzzerPlay(void);

#endif
