#include "buzzer.h"
#include "FreeRTOS.h"
#include "timers.h"

#define BUZZER_GPIO_INSTANCE          GPIOA
#define BUZZER_GPIO_PIN_NUM           GPIO_PIN_1
#define MAX_PERIOD_COUNTER            250   /* MAX_PERIOD_COUNTER * buzzerTimPeriod = sound time */

TimerHandle_t buzzerTimHandler;
uint32_t buzzerTimPeriod = 1;        /* In ms, can't be grater less than 1ms */
uint32_t callbackCounter = 0;

HAL_StatusTypeDef buzzerPlay(void)
{
    BaseType_t retVal;

    retVal = xTimerReset(buzzerTimHandler, 0);
    if (retVal!= pdPASS)
    {
        return HAL_ERROR;
    }
    return HAL_OK;
}

void buzzerStop(void)
{
    BaseType_t retVal;

    retVal = xTimerStop(buzzerTimHandler, 0);
    if (retVal!= pdPASS)
    {
        return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_StatusTypeDef buzzerInit(void)
{
    GPIO_InitTypeDef gpioInit = {0};

    /* Clocks enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /* Pin settings */
    gpioInit.Pin = BUZZER_GPIO_PIN_NUM;
    gpioInit.Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.Pull = GPIO_PULLUP;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BUZZER_GPIO_INSTANCE, &gpioInit);

    /* Create a one-shot timer */
    buzzerTimHandler = xTimerCreate("Buzzer-timer", pdMS_TO_TICKS(buzzerTimPeriod), pdFALSE, 0, buzzerTimCallback);
    if ( buzzerTimHandler == NULL )
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

void buzzerTimCallback(TimerHandle_t xTimer)
{
    HAL_GPIO_TogglePin(BUZZER_GPIO_INSTANCE, BUZZER_GPIO_PIN_NUM);
}
