/**
 ******************************************************************************
 * @file         buzzer.c
 * @author       Aaron Escoboza
 * @brief        Source file for buzzer feature
 ******************************************************************************
 */

#include "buzzer.h"

/* Macros to configure the hardware timer */
#define BUZZER_TIM_PERIOD 0xFFFFFFFF

/* Global variables */
TIM_HandleTypeDef buzzerTimHandler;

/*
 * Description: Start the buzzer by starting the hardware timer.
 */
HAL_StatusTypeDef buzzerStart(void)
{
    HAL_StatusTypeDef halStatus;

    halStatus = HAL_TIM_OC_Start_IT(&buzzerTimHandler, BUZZER_TIM_CHANNEL);
    if (halStatus != HAL_OK)
    {
    }
    return halStatus;
}

/*
 * Description: Stop the buzzer by stopping the hardware timer.
 */
HAL_StatusTypeDef buzzerStop(void)
{
    HAL_StatusTypeDef halStatus;

    halStatus = HAL_TIM_OC_Stop_IT(&buzzerTimHandler, BUZZER_TIM_CHANNEL);
    if (halStatus != HAL_OK)
    {
    }
    return halStatus;
}

/*
 * Description: Initialize TIMER : Output compare mode to produce a square wave.
 */
HAL_StatusTypeDef buzzerInit(void)
{
    HAL_StatusTypeDef halStatus;
    TIM_OC_InitTypeDef timerOCInit = {0};

    /* Timer: Base unit settings */
    buzzerTimHandler.Instance = BUZZER_TIM_INSTANCE;
    buzzerTimHandler.Init.Period = BUZZER_TIM_PERIOD;
    buzzerTimHandler.Init.Prescaler = BUZZER_TIM_PRESCALER;
    buzzerTimHandler.Init.CounterMode = TIM_COUNTERMODE_UP;
    buzzerTimHandler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    halStatus = HAL_TIM_OC_Init(&buzzerTimHandler);
    if (halStatus != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Timer: Channel settings */
    timerOCInit.OCMode = TIM_OCMODE_TOGGLE;
    timerOCInit.OCPolarity = TIM_OCNPOLARITY_HIGH;
    timerOCInit.Pulse = BUZZER_TIM_PULSE_VALUE;
    halStatus = HAL_TIM_OC_ConfigChannel(&buzzerTimHandler, &timerOCInit, BUZZER_TIM_CHANNEL);
    if (halStatus != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Clear interrupt to not jump to the interrupt handler */
    __HAL_TIM_CLEAR_IT(&buzzerTimHandler, TIM_IT_UPDATE);

    return HAL_OK;

}
