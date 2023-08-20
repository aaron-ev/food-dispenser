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

/* Variables to handle button debouncing */
typedef struct 
{ 
    TickType_t timeFalling; 
    TickType_t timeRising; 
    uint8_t edgeRisingDone; 
    uint8_t edgeFallingDone; 
    uint8_t currentState; 
}ButtonDebounce; 

ButtonDebounce enterDebounce = {0, 0, 1, 0, 0};
ButtonDebounce upDebounce = {0, 0, 1, 0, 0};
ButtonDebounce downDebounce = {0, 0, 1, 0, 0};

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

    switch (pin)
    {
        case BUTTON_ENTER_GPIO_PIN:
        {
            if (HAL_GPIO_ReadPin(BUTTON_ENTER_GPIO_PORT, BUTTON_ENTER_GPIO_PIN) == GPIO_PIN_SET)
            {
                enterDebounce.timeRising = xTaskGetTickCount();
                enterDebounce.edgeRisingDone = 1;
                enterDebounce.currentState = GPIO_PIN_SET;
            }
            else
            {
                enterDebounce.timeFalling = xTaskGetTickCount();
                enterDebounce.edgeFallingDone = 1;
                enterDebounce.currentState = GPIO_PIN_RESET;
            }
            if ( (enterDebounce.timeFalling - enterDebounce.timeRising > BUTTON_DEBOUNCE_DELAY) && (enterDebounce.edgeFallingDone == GPIO_PIN_SET) 
                && (enterDebounce.edgeRisingDone == GPIO_PIN_SET) && (enterDebounce.currentState == GPIO_PIN_RESET) )
            {
                xTaskNotifyIndexedFromISR(xTaskDisplayHandler, BUTTON_INDEX_NOTIFICATION,
                                          BUTTON_EVENT_ENTER, eSetBits, &pxHigherPriorityTaskWoken);
                memset(&enterDebounce, 0, sizeof(enterDebounce));
            }
            break;
        }
        case BUTTON_UP_GPIO_PIN:
        {
            if (HAL_GPIO_ReadPin(BUTTON_UP_GPIO_PORT, BUTTON_UP_GPIO_PIN) == GPIO_PIN_SET)
            {
                upDebounce.timeRising = xTaskGetTickCount();
                upDebounce.edgeRisingDone = 1;
                upDebounce.currentState = GPIO_PIN_SET;
            }
            else
            {
                upDebounce.timeFalling = xTaskGetTickCount();
                upDebounce.edgeFallingDone = 1;
                upDebounce.currentState = GPIO_PIN_RESET;
            }
            if ( (upDebounce.timeFalling - upDebounce.timeRising > BUTTON_DEBOUNCE_DELAY) && (upDebounce.edgeFallingDone == GPIO_PIN_SET) 
                && (upDebounce.edgeRisingDone == GPIO_PIN_SET) && (upDebounce.currentState == GPIO_PIN_RESET) )
            {
                xTaskNotifyIndexedFromISR(xTaskDisplayHandler, BUTTON_INDEX_NOTIFICATION,
                                          BUTTON_EVENT_UP, eSetBits, &pxHigherPriorityTaskWoken);
                memset(&upDebounce, 0, sizeof(upDebounce));
            }
            break;
        }
        case BUTTON_DOWN_GPIO_PIN:
        {
            if (HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_PORT, BUTTON_DOWN_GPIO_PIN) == GPIO_PIN_SET)
            {
                downDebounce.timeRising = xTaskGetTickCount();
                downDebounce.edgeRisingDone = 1;
                downDebounce.currentState = GPIO_PIN_SET;
            }
            else
            {
                downDebounce.timeFalling = xTaskGetTickCount();
                downDebounce.edgeFallingDone = 1;
                downDebounce.currentState = GPIO_PIN_RESET;
            }
            if ( (downDebounce.timeFalling - downDebounce.timeRising > BUTTON_DEBOUNCE_DELAY) && (downDebounce.edgeFallingDone == GPIO_PIN_SET) 
                && (downDebounce.edgeRisingDone == GPIO_PIN_SET) && (downDebounce.currentState == GPIO_PIN_RESET) )
            {
                xTaskNotifyIndexedFromISR(xTaskDisplayHandler, BUTTON_INDEX_NOTIFICATION,
                                          BUTTON_EVENT_DOWN, eSetBits, &pxHigherPriorityTaskWoken);
                memset(&downDebounce, 0, sizeof(downDebounce));
            }
            break;
        }
        default:
            PRINT_DEBUG("DEBUG: GPIO callback: Pin source unknown\n");
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

