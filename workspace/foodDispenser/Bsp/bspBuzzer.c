/**
 ******************************************************************************
 * @file         bspBuzzer.c
 * @author       Aaron Escoboza
 * @brief        Source file for buzzer feature
 ******************************************************************************
 */

#include "bspBuzzer.h"

#define BUZZER_TIM_PRESCALER                40  /* tick = 1us*/
#define BUZZER_TIM_PERIOD                   0xFFFFFFFF

TIM_HandleTypeDef buzzerTimHandler;
static uint8_t buzzerTimCh;

/*
 * Description: Start the buzzer by starting the hardware timer.
 */
HAL_StatusTypeDef buzzerStart(void)
{
    HAL_StatusTypeDef halStatus = HAL_OK;
    halStatus = HAL_TIM_OC_Start_IT(&buzzerTimHandler, buzzerTimCh);
    if (halStatus != HAL_OK)
    {
        return HAL_ERROR;
    }
    return halStatus;
}

/*
 * Description: Stop the buzzer by stopping the hardware timer.
 */
HAL_StatusTypeDef buzzerStop(void)
{
    HAL_StatusTypeDef halStatus = HAL_OK;
    halStatus = HAL_TIM_OC_Stop_IT(&buzzerTimHandler, buzzerTimCh);
    if (halStatus != HAL_OK)
    {
        return HAL_ERROR;
    }
    return halStatus;
}


/*
 * Description: Initialize TIMER : Output compare mode to produce a square wave.
 */
HAL_StatusTypeDef buzzerInit(GPIO_TypeDef *GPIOx, uint16_t pin, uint8_t 
                             altFunction, TIM_TypeDef *TIMx, uint32_t timChannel)
{
    HAL_StatusTypeDef halStatus = HAL_OK;
    TIM_OC_InitTypeDef timerOCInit = {0};

    buzzerTimCh = timChannel;
    /* Timer: Base unit settings */
    buzzerTimHandler.Instance = TIMx;
    buzzerTimHandler.Init.Period = BUZZER_TIM_PERIOD;
    buzzerTimHandler.Init.Prescaler = BUZZER_TIM_PRESCALER;
    buzzerTimHandler.Init.CounterMode = TIM_COUNTERMODE_UP;
    buzzerTimHandler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    halStatus = HAL_TIM_OC_Init(&buzzerTimHandler);
    if (halStatus != HAL_OK)
    {
        goto error_buzzer_init;
    }

    /* Timer: Channel settings */
    timerOCInit.OCMode = TIM_OCMODE_TOGGLE;
    timerOCInit.OCPolarity = TIM_OCNPOLARITY_HIGH;
    timerOCInit.Pulse = BUZZER_TIM_PULSE_VALUE;
    halStatus = HAL_TIM_OC_ConfigChannel(&buzzerTimHandler, &timerOCInit, timChannel);
    if (halStatus != HAL_OK)
    {
        goto error_buzzer_init;
    }

    /* Clear interrupt to not jump to the interrupt handler */
    __HAL_TIM_CLEAR_IT(&buzzerTimHandler, TIM_IT_UPDATE);

    return halStatus;
error_buzzer_init:
    return HAL_ERROR; 
}
