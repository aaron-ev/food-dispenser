/**
  ******************************************************************************
  * @file    bspServomotor.h
  * @author  Aaron Escoboza
  * @brief   Header file to manage the servomotor
  ******************************************************************************
  */

#ifndef __BSP_SERVOMOTOR__H
#define __BSP_SERVOMOTOR__H

#include "stm32f4xx_hal.h"

typedef enum
{
    SERVOMOTOR_DEGREES_0,
    SERVOMOTOR_DEGREES_180
}ServoMotorDegree;

void servoMotorStop(void);
void servoMotorStart(void);
HAL_StatusTypeDef servoMotorInit(GPIO_TypeDef *GPIOx, uint16_t pin, uint8_t 
                             altFunction, TIM_TypeDef *TIMx, uint32_t timChannel);
void servoMotorSetDegrees(ServoMotorDegree position);

#endif
