

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

#define SERVO_MOTOR_TIM_BASE_PRESCALER          15    /* */
#define SERVO_MOTOR_TIM_BASE_PERIOD             20000 /* freq = 50 Hz */
#define SERVO_MOTOR_2MS_SIGNAL                  ((SERVO_MOTOR_TIM_BASE_PERIOD * 10) / 100)
#define SERVO_MOTOR_1_5MS_SIGNAL                ((SERVO_MOTOR_TIM_BASE_PERIOD * 7.5) / 100)
#define SERVO_MOTOR_1MS_SIGNAL                  ((SERVO_MOTOR_TIM_BASE_PERIOD * 5) / 100)

TIM_HandleTypeDef servoMotorTimHandler = {0};

HAL_StatusTypeDef servoMotorInit(GPIO_TypeDef *GPIOx, uint32_t pin, uint8_t channel)
{
    GPIO_InitTypeDef servoMotorGpioInit = {0};
    TIM_OC_InitTypeDef servoMotorChannelConfing = {0};
    HAL_StatusTypeDef halStatus;

    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* GPIO settings: ServoMotor */
    servoMotorGpioInit.Pin = pin;
    servoMotorGpioInit.Mode = GPIO_MODE_AF_PP;
    servoMotorGpioInit.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOx, &servoMotorGpioInit);

    /*TIMER base unit settings: Servo motor */
    servoMotorTimHandler.Instance = SERVO_MOTOR_TIM_INSTANCE;
    servoMotorTimHandler.Init.Prescaler = SERVO_MOTOR_TIM_BASE_PRESCALER;
    servoMotorTimHandler.Init.Period = SERVO_MOTOR_TIM_BASE_PERIOD;
    halStatus = HAL_TIM_PWM_Init(&servoMotorTimHandler);
    if (halStatus != HAL_OK)
    {
    }

    /* TIMER channel settings: Servo motor */
    servoMotorChannelConfing.OCMode = TIM_OCMODE_PWM1;
    servoMotorChannelConfing.OCPolarity = TIM_OCPOLARITY_HIGH;
    servoMotorChannelConfing.Pulse = SERVO_MOTOR_2MS_SIGNAL;
    halStatus = HAL_TIM_PWM_ConfigChannel(&servoMotorTimHandler, &servoMotorChannelConfing, channel);
    if (halStatus != HAL_OK)
    {
    }

    return halStatus;
}

void servoMotorStart(void)
{
    HAL_TIM_PWM_Start(&servoMotorTimHandler, TIM_CHANNEL_1);
}

void servoMotorStop(void)
{
    HAL_TIM_PWM_Stop(&servoMotorTimHandler, TIM_CHANNEL_1);
}

void servoMotorRotate(ServoPosition position)
{
    switch (position)
    {
        case SERVO_MOTOR_DEGREES_0: __HAL_TIM_SET_COMPARE(&servoMotorTimHandler, TIM_CHANNEL_1,
                                                          SERVO_MOTOR_2MS_SIGNAL);
                                    break;
        case SERVO_MOTOR_DEGREES_90: __HAL_TIM_SET_COMPARE(&servoMotorTimHandler, TIM_CHANNEL_1,
                                                           SERVO_MOTOR_1_5MS_SIGNAL);
                                    break;
        case SERVO_MOTOR_DEGREES_180: __HAL_TIM_SET_COMPARE(&servoMotorTimHandler, TIM_CHANNEL_1,
                                                            SERVO_MOTOR_1MS_SIGNAL);
                                    break;
        default : break;
    }
}
