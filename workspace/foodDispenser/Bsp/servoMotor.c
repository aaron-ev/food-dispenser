/**
  ******************************************************************************
  * @file    servomotor.c
  * @author  Aaron Escoboza
  * @brief   Provide servo motor APIs based on a hardware timber configures in
  *          PWM.
  ******************************************************************************
  */

#include "servoMotor.h"
#include "appConfig.h"

#define SERVO_MOTOR_TIM_BASE_PRESCALER          40    /* 1us each tick*/
#define SERVO_MOTOR_TIM_BASE_PERIOD             20000 /* freq = 50 Hz */
#define SERVO_MOTOR_2MS_SIGNAL                  ((SERVO_MOTOR_TIM_BASE_PERIOD * 10) / 100)
#define SERVO_MOTOR_1_5MS_SIGNAL                ((SERVO_MOTOR_TIM_BASE_PERIOD * 7.5) / 100)
#define SERVO_MOTOR_1MS_SIGNAL                  ((SERVO_MOTOR_TIM_BASE_PERIOD * 5) / 100)
#define SERVO_MOTOR_DELAY                       250

TIM_HandleTypeDef servoMotorTimHandler = {0};

void servoMotorStart(void);
void servoMotorStop(void);
void servoMotorRotate(Degrees_E position);

void servoMotorStart(void)
{
    HAL_TIM_PWM_Start(&servoMotorTimHandler, SERVO_MOTOR_TIM_CHANNEL);
}

void servoMotorStop(void)
{
    HAL_TIM_PWM_Stop(&servoMotorTimHandler, SERVO_MOTOR_TIM_CHANNEL);
}

void servoMotorSetDegrees(Degrees_E position)
{
    if (position == SERVO_MOTOR_DEGREES_0)
    {
        __HAL_TIM_SET_COMPARE(&servoMotorTimHandler, SERVO_MOTOR_TIM_CHANNEL,
                              SERVO_MOTOR_2MS_SIGNAL);
    }
    if(position == SERVO_MOTOR_DEGREES_180)
    {
        __HAL_TIM_SET_COMPARE(&servoMotorTimHandler, SERVO_MOTOR_TIM_CHANNEL,
                              SERVO_MOTOR_1MS_SIGNAL);
    }
}

HAL_StatusTypeDef servoMotorInit(void)
{
    HAL_StatusTypeDef halStatus;
    TIM_OC_InitTypeDef servoMotorChannelConfig = {0};

    /* TIMER base unit settings: Servo motor */
    servoMotorTimHandler.Instance = SERVO_MOTOR_TIM_INSTANCE;
    servoMotorTimHandler.Init.Prescaler = SERVO_MOTOR_TIM_BASE_PRESCALER;
    servoMotorTimHandler.Init.Period = SERVO_MOTOR_TIM_BASE_PERIOD;
    halStatus = HAL_TIM_PWM_Init(&servoMotorTimHandler);
    if (halStatus != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* TIMER channel settings: Servo motor */
    servoMotorChannelConfig.OCMode = TIM_OCMODE_PWM1;
    servoMotorChannelConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    servoMotorChannelConfig.Pulse = SERVO_MOTOR_2MS_SIGNAL;
    halStatus = HAL_TIM_PWM_ConfigChannel(&servoMotorTimHandler, &servoMotorChannelConfig, SERVO_MOTOR_TIM_CHANNEL);
    if (halStatus != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}
