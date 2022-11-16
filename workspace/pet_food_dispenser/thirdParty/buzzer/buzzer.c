


#include "buzzer.h"



HAL_StatusTypeDef buzzerInit(Buzzer *buzzer)
{
    TIM_OC_InitTypeDef channelConfig;

    /* Initialize base unit */
    buzzer->timerHandler = BUZZER_TIM_INSTANCE;
    buzzer->timerHandler.Channel = buzzer->channel;
    buzzer->timerHandler.Init.Prescaler = TIM_CLOCKPRESCALER_DIV1;
    buzzer->timerHandler.Init.Period = 0;
    buzzer->timerHandler.Init.CounterMode = TIM_COUNTERMODE_UP;
    if (HAL_TIM_OC_Init(buzzer->timerHandler) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Configure the channel */
    channelConfig.OCMode = ;
    channelConfig.OCNPolarity = ;
    channelConfig.Pulse = ;

    if (HAL_TIM_OC_ConfigChannel(&(buzzer->timerHandler), &channelConfig, buzzer->channel) != HAL_OK)
    {
        return HAL_ERROR;
    }
}

HAL_StatusTypeDef buzzerEnable(Buzzer *buzzer)
{
    if (HAL_TIM_OC_Start(buzzer->timerHandler, buzzer->channel) != HAL_OK)
    {
        return HAL_ERROR;
    }
}

HAL_StatusTypeDef buzzerDisable(Buzzer *buzzer)
{
    if (HAL_TIM_OC_Stop(buzzer->timerHandler, buzzer->channel) != HAL_OK)
    {
        return HAL_ERROR;
    }
}
