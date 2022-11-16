#include "buzzer.h"

HAL_StatusTypeDef buzzerInit(void)
{
//    TIM_OC_InitTypeDef channelConfig;
//
//    if (buzzer == NULL )
//    {
//        return HAL_ERROR;
//    }
//
//    if (buzzer->gpioInstance == NULL)
//    {
//        return HAL_ERROR;
//    }
//
//
//    /* Initialize base unit */
//    buzzer->timerHandler.Init.Prescaler = TIM_CLOCKPRESCALER_DIV1;
//    buzzer->timerHandler.Init.Period = 0;
//    buzzer->timerHandler.Init.CounterMode = TIM_COUNTERMODE_UP;
//    if (HAL_TIM_OC_Init(&buzzer->timerHandler) != HAL_OK)
//    {
//        return HAL_ERROR;
//    }
//
//    /* Configure the channel */
//    channelConfig.OCMode = TIM_OCMODE_TOGGLE;
//    channelConfig.OCPolarity = TIM_OCNPOLARITY_HIGH;
//    channelConfig.Pulse = 0;// change value
//
//    if (HAL_TIM_OC_ConfigChannel(&buzzer->timerHandler, &channelConfig, buzzer->timerHandler.Channel) != HAL_OK)
//    {
//        return HAL_ERROR;
//    }
//
//    return HAL_OK;
}

HAL_StatusTypeDef buzzerEnable(void)
{
    if (BUZZER_TIMER_INSTANCE == NULL)
    {
        return HAL_ERROR;
    }
    if (HAL_TIM_OC_Start(BUZZER_TIMER_INSTANCE, BUZZER_TIMER_CHANNEL) != HAL_OK)
    {
        return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_StatusTypeDef buzzerDisable(void)
{
    if (BUZZER_TIMER_INSTANCE == NULL)
    {
        return HAL_ERROR;
    }
    if (HAL_TIM_OC_Stop(BUZZER_TIMER_INSTANCE, BUZZER_TIMER_CHANNEL) != HAL_OK)
    {
        return HAL_ERROR;
    }
    return HAL_OK;
}
