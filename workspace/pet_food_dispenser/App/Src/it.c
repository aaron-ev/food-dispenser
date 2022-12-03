/**
 ******************************************************************************
 * @file         it.c
 * @author       Aaron Escoboza
 * @brief        Include interrupt handles implementation
 ******************************************************************************
 */

#include "stm32f4xx_hal.h"
#include "appConfig.h"

extern TIM_HandleTypeDef buzzerTimHandler;

/*
* Interrupt handler for the buzzer timer.
*/
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&buzzerTimHandler);
}

/*
 *  Callback to handle update events and capture compare mode
 */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *timerHandler)
{
    static uint32_t ccr1Reg;

    if (timerHandler->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        ccr1Reg = HAL_TIM_ReadCapturedValue(timerHandler, BUZZER_TIM_CHANNEL);
        __HAL_TIM_SET_COMPARE(timerHandler, BUZZER_TIM_CHANNEL, (ccr1Reg + BUZZER_TIM_PULSE_VALUE));
    }
}
