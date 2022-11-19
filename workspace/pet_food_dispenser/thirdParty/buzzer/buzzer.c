#include "buzzer.h"
#include "FreeRTOS.h"
#include "timers.h"

TIM_HandleTypeDef timHandler;

volatile uint32_t pulse_value = 16000; //to produce 100Khz
volatile uint32_t ccr;

#define BUZZER_GPIO_INSTANCE          GPIOA
#define BUZZER_GPIO_PIN_NUM           GPIO_PIN_1
#define MAX_PERIOD_COUNTER 500

TIM_HandleTypeDef timHandler;
TimerHandle_t buzzerTimHandler;
static uint32_t buzzerTimPeriod = 1;    /* ms */
static uint32_t callbackCounter = 0;

void buzzerTimCallback(TimerHandle_t xTimer)
{
    if ( callbackCounter <= MAX_PERIOD_COUNTER )
    {
        HAL_GPIO_TogglePin(BUZZER_GPIO_INSTANCE, BUZZER_GPIO_PIN_NUM);
        if (xTimerReset(buzzerTimHandler, 0) != pdPASS)
        {
            //todo: try, handle error.
        }
        callbackCounter++;
    }
}

HAL_StatusTypeDef buzzerPlay(void)
{
    callbackCounter = 0;
    if (xTimerReset(buzzerTimHandler, 0) != pdPASS)
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

    /* Create a one-shot timer to produce a square wave for buzzer sound */
    buzzerTimHandler = xTimerCreate("Buzzer-timer", pdMS_TO_TICKS(buzzerTimPeriod), pdFALSE, 0, buzzerTimCallback);
    if ( buzzerTimHandler == NULL )
    {
        return HAL_ERROR;
    }
    buzzerPlay();
    return HAL_OK;
}
