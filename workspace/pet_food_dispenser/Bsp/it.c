/**
 ******************************************************************************
 * @file         it.c
 * @author       Aaron Escoboza
 * @brief        Include interrupt handles implementation
 ******************************************************************************
 */

#include "appConfig.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

extern TIM_HandleTypeDef buzzerTimHandler;
extern TaskHandle_t xTaskDisplayHandler;

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

/*
 * Interrupt handler for push button ENTER.
 */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(BUTTON_ENTER_GPIO_PIN);
}

/*
 * Interrupt handler for push button UP.
 */
void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(BUTTON_UP_GPIO_PIN);
}

/*
 * Interrupt handler for push button DOWN.
 */
void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(BUTTON_DOWN_GPIO_PIN);
}

/*
 * Callback to handle push button requests.
 */
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    BaseType_t  pxHigherPriorityTaskWoken = pdFALSE;

    if (pin == BUTTON_ENTER_GPIO_PIN)
    {
        xTaskNotifyIndexedFromISR(xTaskDisplayHandler, BUTTON_INDEX_NOTIFICATION, BUTTON_EVENT_ENTER, eSetBits, &pxHigherPriorityTaskWoken);
    }
    if (pin == BUTTON_UP_GPIO_PIN)
    {
        xTaskNotifyIndexedFromISR(xTaskDisplayHandler, BUTTON_INDEX_NOTIFICATION, BUTTON_EVENT_UP, eSetBits, &pxHigherPriorityTaskWoken);
    }
    if (pin == BUTTON_DOWN_GPIO_PIN)
    {
        xTaskNotifyIndexedFromISR(xTaskDisplayHandler, BUTTON_INDEX_NOTIFICATION, BUTTON_EVENT_DOWN, eSetBits, &pxHigherPriorityTaskWoken);
    }
}
