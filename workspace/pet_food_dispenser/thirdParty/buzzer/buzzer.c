#include "buzzer.h"

TIM_HandleTypeDef timHandler;

HAL_StatusTypeDef buzzerInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_OC_InitTypeDef channelConfig;

    /* Clocks enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_TIM2_CLK_ENABLE();
    /* Pin settings */
    GPIO_InitStruct.Pin = BUZZER_GPIO_PIN_NUM;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(BUZZER_GPIO_INSTANCE, &GPIO_InitStruct);
    /* Timer settings */
    timHandler.Instance = BUZZER_TIMER_INSTANCE;
    timHandler.Channel = BUZZER_TIMER_CHANNEL;
    timHandler.Init.Prescaler = TIM_CLOCKDIVISION_DIV1;
    timHandler.Init.Period = 0xFFFFFFFF;
    timHandler.Init.CounterMode = TIM_COUNTERMODE_UP;
    if (HAL_TIM_OC_Init(&timHandler) != HAL_OK)
    {
        return HAL_ERROR;
    }
    /* Channel settings */
    channelConfig.OCMode = TIM_OCMODE_TOGGLE;
    channelConfig.OCPolarity = TIM_OCNPOLARITY_HIGH;
    channelConfig.Pulse = 0;
    if (HAL_TIM_OC_ConfigChannel(&timHandler, &channelConfig, BUZZER_TIMER_CHANNEL) != HAL_OK)
    {
        return HAL_ERROR;
    }
    /* Interrupt settings */
    HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

   return HAL_OK;
}

HAL_StatusTypeDef buzzerEnable(void)
{
    if (HAL_TIM_OC_Start(&timHandler, BUZZER_TIMER_CHANNEL) != HAL_OK)
    {
        return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_StatusTypeDef buzzerDisable(void)
{
    if (HAL_TIM_OC_Stop(&timHandler, BUZZER_TIMER_CHANNEL) != HAL_OK)
    {
        return HAL_ERROR;
    }
    return HAL_OK;
}

 void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
 {

 }
