/**

  ******************************************************************************
  * @file    servomotor.h
  * @author  Aaron Escoboza
  * @brief   Header file to manage the a servomotor.h
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#ifndef __SERVOMOTOR__H
#define __SERVOMOTOR__H

#include "stm32f4xx_hal.h"

typedef enum
{
    SERVO_MOTOR_DEGREES_0,
    SERVO_MOTOR_DEGREES_90,
    SERVO_MOTOR_DEGREES_180
}ServoPosition;

HAL_StatusTypeDef servoMotorInit(void);
void servoMotorRotate(ServoPosition position);

#endif
