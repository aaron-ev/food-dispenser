

/**
  ******************************************************************************
  * @file    servomotor.c
  * @author  Aaron Escoboza
  * @brief   Provide servo motor APIs based on a hardware timber configures in
  *          PWM.
  ******************************************************************************
  * @attention
  *
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

extern void errorHandler(void);
TIM_HandleTypeDef servoMotorTimHandler = {0};

void servoMotorStart(void);
void servoMotorStop(void);
void servoMotorRotate(ServoPosition position);

void servoMotorStart(void)
{
    HAL_TIM_PWM_Start(&servoMotorTimHandler, SERVO_MOTOR_TIM_CHANNEL);
}

void servoMotorStop(void)
{
    HAL_TIM_PWM_Stop(&servoMotorTimHandler, SERVO_MOTOR_TIM_CHANNEL);
}

HAL_StatusTypeDef servoMotorInit(void)
{
    TIM_OC_InitTypeDef servoMotorChannelConfing = {0};
    HAL_StatusTypeDef halStatus;

    /* TIMER base unit settings: Servo motor */
    servoMotorTimHandler.Instance = SERVO_MOTOR_TIM_INSTANCE;
    servoMotorTimHandler.Init.Prescaler = SERVO_MOTOR_TIM_BASE_PRESCALER;
    servoMotorTimHandler.Init.Period = SERVO_MOTOR_TIM_BASE_PERIOD;
    halStatus = HAL_TIM_PWM_Init(&servoMotorTimHandler);
    if (halStatus != HAL_OK)
    {
        errorHandler();
    }

    /* TIMER channel settings: Servo motor */
    servoMotorChannelConfing.OCMode = TIM_OCMODE_PWM1;
    servoMotorChannelConfing.OCPolarity = TIM_OCPOLARITY_HIGH;
    servoMotorChannelConfing.Pulse = SERVO_MOTOR_2MS_SIGNAL;
    halStatus = HAL_TIM_PWM_ConfigChannel(&servoMotorTimHandler, &servoMotorChannelConfing, SERVO_MOTOR_TIM_CHANNEL);
    if (halStatus != HAL_OK)
    {
        errorHandler();
    }

    /* Go to the default state */
    servoMotorRotate(SERVO_MOTOR_DEGREES_0);

    return halStatus;
}

void servoMotorRotate(ServoPosition position)
{
    switch (position)
    {
        case SERVO_MOTOR_DEGREES_0: __HAL_TIM_SET_COMPARE(&servoMotorTimHandler, SERVO_MOTOR_TIM_CHANNEL,
                                                          SERVO_MOTOR_2MS_SIGNAL);
                                    break;
        case SERVO_MOTOR_DEGREES_90: __HAL_TIM_SET_COMPARE(&servoMotorTimHandler, SERVO_MOTOR_TIM_CHANNEL,
                                                           SERVO_MOTOR_1_5MS_SIGNAL);
                                    break;
        case SERVO_MOTOR_DEGREES_180: __HAL_TIM_SET_COMPARE(&servoMotorTimHandler, SERVO_MOTOR_TIM_CHANNEL,
                                                            SERVO_MOTOR_1MS_SIGNAL);
                                    break;
        default : break;
    }
    servoMotorStart();
//    HAL_Delay(SERVO_MOTOR_DELAY);
//    servoMotorStop();
}
