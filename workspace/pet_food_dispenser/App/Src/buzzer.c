#include "buzzer.h"
#include "FreeRTOS.h"
#include "timers.h"

#define BUZZER_GPIO_INSTANCE          GPIOA
#define BUZZER_GPIO_PIN_NUM           GPIO_PIN_1
#define MAX_PERIOD_COUNTER            250   /* MAX_PERIOD_COUNTER * buzzerTimPeriod = sound time */

TimerHandle_t buzzerTimHandler;

HAL_StatusTypeDef buzzerStart(void)
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

HAL_StatusTypeDef buzzerInit(uint32_t buzzerTimPeriod, uint32_t pin, GPIO_TypeDef *GPIOx)
{
    GPIO_InitTypeDef gpioInit = {0};

    /* Clocks enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /* Pin settings */
    gpioInit.Pin = pin;
    gpioInit.Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.Pull = GPIO_PULLUP;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOx, &gpioInit);

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
