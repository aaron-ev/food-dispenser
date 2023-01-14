/**
  ******************************************************************************
  * @file    servomotor.h
  * @author  Aaron Escoboza
  * @brief   Header file to manage the a servomotor.h
  ******************************************************************************
  */

#ifndef __SERVOMOTOR__H
#define __SERVOMOTOR__H

#include "stm32f4xx_hal.h"

typedef enum
{
    SERVO_MOTOR_DEGREES_0,
    SERVO_MOTOR_DEGREES_180
}Degrees_E;

void servoMotorStop(void);
void servoMotorStart(void);
HAL_StatusTypeDef servoMotorInit(void);
void servoMotorSetDegrees(Degrees_E position);

#endif
