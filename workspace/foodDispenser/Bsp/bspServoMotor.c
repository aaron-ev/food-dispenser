/**
  ******************************************************************************
  * @file    servomotor.c
  * @author  Aaron Escoboza
  * @brief   Provide servo motor APIs based on a hardware timber configures in
  *          PWM.
  ******************************************************************************
  */

#include "bspServoMotor.h"

/* Prescaler and period calculated based on PB1 = 40MHz*/
#define SERVOMOTOR_TIM_BASE_PRESCALER          24
#define SERVOMOTOR_TIM_BASE_PERIOD             32000
#define SERVOMOTOR_2MS_SIGNAL                  ((SERVOMOTOR_TIM_BASE_PERIOD * 12.5) / 100)
#define SERVOMOTOR_1_5MS_SIGNAL                ((SERVOMOTOR_TIM_BASE_PERIOD * 7.5) / 100)
#define SERVOMOTOR_1MS_SIGNAL                  ((SERVOMOTOR_TIM_BASE_PERIOD * 2.5) / 100)

TIM_HandleTypeDef servoMotorTimHandler = {0};
static uint8_t servomotorTimCh;

void servoMotorStart(void);
void servoMotorStop(void);

void servoMotorStart(void)
{
    HAL_TIM_PWM_Start(&servoMotorTimHandler, servomotorTimCh);
}

void servoMotorStop(void)
{
    HAL_TIM_PWM_Stop(&servoMotorTimHandler, servomotorTimCh);
}

void servoMotorSetDegrees(ServoMotorDegree degrees)
{
    uint32_t compareVal;

    compareVal = (degrees == SERVOMOTOR_DEGREES_0) ? SERVOMOTOR_2MS_SIGNAL:
                                                      SERVOMOTOR_1MS_SIGNAL;
    __HAL_TIM_SET_COMPARE(&servoMotorTimHandler, servomotorTimCh, compareVal);
}

HAL_StatusTypeDef servoMotorInit(GPIO_TypeDef *GPIOx, uint16_t pin, uint8_t 
                             altFunction, TIM_TypeDef *TIMx, uint32_t timChannel)
{
    HAL_StatusTypeDef halStatus = HAL_OK;
    TIM_OC_InitTypeDef servoMotorChannelConfig = {0};

    /* TIMER base unit settings: Servo motor */
    servoMotorTimHandler.Instance = TIMx;
    servoMotorTimHandler.Init.Prescaler = SERVOMOTOR_TIM_BASE_PRESCALER;
    servoMotorTimHandler.Init.Period = SERVOMOTOR_TIM_BASE_PERIOD - 1;
    halStatus = HAL_TIM_PWM_Init(&servoMotorTimHandler);
    if (halStatus != HAL_OK)
    {
    	goto error_servoMotorInit;
    }

    /* TIMER channel settings: Servo motor */
    servoMotorChannelConfig.OCMode = TIM_OCMODE_PWM1;
    servoMotorChannelConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    servoMotorChannelConfig.Pulse = SERVOMOTOR_2MS_SIGNAL;
    halStatus = HAL_TIM_PWM_ConfigChannel(&servoMotorTimHandler, &servoMotorChannelConfig, timChannel);
    if (halStatus != HAL_OK)
    {
    	goto error_servoMotorInit;
    }

    return halStatus;
error_servoMotorInit:
	return HAL_ERROR;
}
