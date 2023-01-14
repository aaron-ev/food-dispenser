/**
 ******************************************************************************
 * @file         it.c
 * @author       Aaron Escoboza
 * @brief        Include interrupt handles implementation
 ******************************************************************************
 */

#include "appConfig.h"
#include "main.h"
#include "buzzer.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "console.h"

extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef buzzerTimHandler;
extern TaskHandle_t xTaskDisplayHandler;
extern uint8_t debounceTmrActive;
extern TimerHandle_t xDebounceTmrHandler;

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
 * Interrupt handler for the TOUCH IRQ
 */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/*
 * Interrupt handler for push button ENTER.
 */
void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(BUTTON_ENTER_GPIO_PIN);
}

/*
 * Interrupt handler for push button UP.
 */
void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(BUTTON_UP_GPIO_PIN);
}

/*
 * Interrupt handler for push button DOWN.
 */
void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(BUTTON_DOWN_GPIO_PIN);
}

/*
 * Callback to handle push button requests.
 */
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    BaseType_t  pxHigherPriorityTaskWoken = pdFALSE;

    if (debounceTmrActive)
    {
        portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        PRINT_DEBUG("DEBUG: GPIO callback:Debounce timer active \n");
        return;
    }

    if (xTimerResetFromISR(xDebounceTmrHandler, &pxHigherPriorityTaskWoken) != pdTRUE)
    {
        consolePrint("DEBUG: GPIO callback: debounce timer could not be started \n");
    }
    debounceTmrActive = 1;
    PRINT_DEBUG("DEBUG: GPIO callback:Debounce timer started \n");

    switch (pin)
    {
        case BUTTON_ENTER_GPIO_PIN:
            xTaskNotifyIndexedFromISR(xTaskDisplayHandler, BUTTON_INDEX_NOTIFICATION,
                                      BUTTON_EVENT_ENTER, eSetBits, &pxHigherPriorityTaskWoken);
            break;
        case BUTTON_UP_GPIO_PIN:
                xTaskNotifyIndexedFromISR(xTaskDisplayHandler, BUTTON_INDEX_NOTIFICATION,
                                          BUTTON_EVENT_UP, eSetBits, &pxHigherPriorityTaskWoken);
            break;
        case BUTTON_DOWN_GPIO_PIN:
                xTaskNotifyIndexedFromISR(xTaskDisplayHandler, BUTTON_INDEX_NOTIFICATION,
                                          BUTTON_EVENT_DOWN, eSetBits, &pxHigherPriorityTaskWoken);
            break;
        default:
            PRINT_DEBUG("DEBUG: GPIO callback: Pin source unkown\n");
            break;
    }
}

/*
 *  Interrupt handler for the HAL time base.
 */
void TIM1_BRK_TIM9_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim9);
}

