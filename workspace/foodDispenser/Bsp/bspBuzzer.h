/**
  ******************************************************************************
  * @file    bspBuzzer.h
  * @author  Aaron Escoboza
  * @brief   Header file to manage the a buzzer.
  ******************************************************************************
*/

#ifndef __BSP_BUZZER__H
#define __BSP_BUZZER_H

// #include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
// #include "appConfig.h"
#include "stdint.h"

#define BUZZER_TIM_PULSE_VALUE              500 /*freq = 1khz, 1us * 500 * 2 */

HAL_StatusTypeDef buzzerInit(GPIO_TypeDef *GPIOx, uint16_t pin, uint8_t 
                             altFunction, TIM_TypeDef *TIMx, uint32_t timChannel);
HAL_StatusTypeDef buzzerStart(void);
HAL_StatusTypeDef buzzerStop(void);

#endif
