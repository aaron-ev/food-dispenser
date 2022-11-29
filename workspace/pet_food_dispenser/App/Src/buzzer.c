#include "buzzer.h"

/* Macros to configure the hardware timer */
#define BUZZER_TIM_PERIOD 0xFFFFFFFF
#define BUZZER_TIM_PRESCALER 1
#define BUZZER_TIM_PULSE_VALUE 2000

/* Timer variables */
TIM_HandleTypeDef timHandler;

HAL_StatusTypeDef buzzerStart(uint32_t channel)
{
    HAL_StatusTypeDef halStatus;

    halStatus = HAL_TIM_OC_Start_IT(&timHandler, channel);
    if (halStatus != HAL_OK)
    {
    }
    return halStatus;
}

HAL_StatusTypeDef buzzerStop(uint32_t channel)
{
    HAL_StatusTypeDef halStatus;

    halStatus = HAL_TIM_OC_Start_IT(&timHandler, channel);
    if (halStatus != HAL_OK)
    {
    }
    return halStatus;
}

/*
 * Description: Initialize GPIO and TIMER settings
 */
HAL_StatusTypeDef buzzerInit(GPIO_TypeDef *GPIOx, uint32_t pin, TIM_TypeDef *TIMx, uint32_t channel)
{
    GPIO_InitTypeDef gpioInit = {0};
    TIM_OC_InitTypeDef timInit;
    HAL_StatusTypeDef halStatus = HAL_OK;

    /* GPIO settings */
    gpioInit.Pin = pin;
    gpioInit.Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.Pull = GPIO_PULLUP;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOx, &gpioInit);

    /* Timer: Base unit settings */
    timHandler.Instance = TIMx;
    timHandler.Init.Period = BUZZER_TIM_PERIOD;
    timHandler.Init.Prescaler = BUZZER_TIM_PULSE_VALUE;
    halStatus = HAL_TIM_OC_Init(&timHandler);
    if (halStatus != HAL_OK)
    {
    }

    /* Timer: Channel settings */
    timInit.OCMode = TIM_OCMODE_TOGGLE;
    timInit.OCPolarity = TIM_OCPOLARITY_HIGH;
    // timInit.Pulse = pulse1_value;
    timInit.Pulse = BUZZER_TIM_PULSE_VALUE;
    halStatus = HAL_TIM_OC_ConfigChannel(&timHandler, &timInit, channel);
    if (halStatus != HAL_OK)
    {
    }

    return halStatus;
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint32_t ccr;

    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        ccr = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, (ccr + BUZZER_TIM_PULSE_VALUE));
    }
}