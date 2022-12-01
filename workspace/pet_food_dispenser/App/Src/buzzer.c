#include "buzzer.h"

/* Macros to configure the hardware timer */
#define BUZZER_TIM_PERIOD 0xFFFFFFFF

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

    halStatus = HAL_TIM_OC_Stop_IT(&timHandler, channel);
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
    TIM_OC_InitTypeDef timOCInit = {0};
    GPIO_InitTypeDef gpioInit = {0};
    HAL_StatusTypeDef halStatus = HAL_OK;

    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /* GPIO settings */
    gpioInit.Pin = pin;
    gpioInit.Mode = GPIO_MODE_AF_PP;
    gpioInit.Pull = GPIO_NOPULL;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    gpioInit.Alternate = GPIO_AF1_TIM2;

    HAL_GPIO_Init(GPIOx, &gpioInit);
    /* Timer: Base unit settings */
    timHandler.Instance = TIMx;
    timHandler.Init.Period = BUZZER_TIM_PERIOD;
    timHandler.Init.Prescaler = BUZZER_TIM_PRESCALER;
    timHandler.Init.CounterMode = TIM_COUNTERMODE_UP;
    timHandler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    halStatus = HAL_TIM_OC_Init(&timHandler);
    if (halStatus != HAL_OK)
    {
    }

    /* Timer: Channel settings */
    timOCInit.OCMode = TIM_OCMODE_TOGGLE;
    timOCInit.OCPolarity = TIM_OCNPOLARITY_HIGH;
    timOCInit.Pulse = BUZZER_TIM_PULSE_VALUE;
    halStatus = HAL_TIM_OC_ConfigChannel(&timHandler, &timOCInit, channel);
    if (halStatus != HAL_OK)
    {
    }

    __HAL_TIM_CLEAR_IT(&timHandler, TIM_IT_UPDATE);
    HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    return halStatus;
}

void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&timHandler);
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
