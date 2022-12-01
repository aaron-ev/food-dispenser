

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

#define SERVO_MOTOR_TIM_BASE_PRESCALER          80 - 1 /* newClk = 100 kHz, P2CLK = 8 Mhz */
#define SERVO_MOTOR_TIM_BASE_PERIOD             2000 /* freq = 50 Hz */

TIM_HandleTypeDef servoMotorTimHandler = {0};

HAL_StatusTypeDef servoMotorInit(GPIO_TypeDef *GPIOx, uint16_t pin, uint8_t channel)
{
    GPIO_InitTypeDef servoMotorGpioInit = {0};
    TIM_OC_InitTypeDef servoMotorChannelConfing = {0};
    HAL_StatusTypeDef halStatus;

    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* GPIO settings: ServoMotor */
    servoMotorGpioInit.Pin = pin;
    servoMotorGpioInit.Mode = GPIO_MODE_AF_PP;
    servoMotorGpioInit.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOx, &servoMotorGpioInit);

    /*TIMER base unit settings: Servo motor */
    servoMotorTimHandler.Instance = SERVO_MOTOR_INSTANCE;
    servoMotorTimHandler.Init.Prescaler = SERVO_MOTOR_TIM_BASE_PRESCALER;
    servoMotorTimHandler.Init.Period = SERVO_MOTOR_TIM_BASE_PERIOD;
    halStatus = HAL_TIM_PWM_Init(&servoMotorTimHandler);
    if (halStatus != HAL_OK)
    {
    }

    /* TIMER channel settings: Servo motor */
    servoMotorChannelConfing.OCMode = TIM_OCMODE_PWM1;
    servoMotorChannelConfing.OCPolarity = TIM_OCPOLARITY_HIGH;
    servoMotorChannelConfing.Pulse = SERVO_MOTOR_DEGREES_0;
    halStatus = HAL_TIM_PWM_ConfigChannel(&servoMotorTimHandler, &servoMotorChannelConfing, channel);
    if (halStatus != HAL_OK)
    {
    }

    return halStatus;
}

void servoMotorRotate(ServoPosition position)
{
    switch (position)
    {
        case SERVO_MOTOR_DEGREES_0: //__HAL_TIM_SET_COMPARE(&tim2PWM,TIM_CHANNEL_1,SERVO_SIGNAL_2ms);
                              break;
        case SERVO_MOTOR_DEGREES_90: //__HAL_TIM_SET_COMPARE(&tim2PWM,TIM_CHANNEL_1,SERVO_SIGNAL_0_5ms);
                             break;
        case SERVO_MOTOR_DEGREES_180: //__HAL_TIM_SET_COMPARE(&tim2PWM,TIM_CHANNEL_1,SERVO_SIGNAL_0_5ms);
                             break;
        default : break;
    }
}
