
/**************************************************************************
 * Author: Aaron Escoboza
 * Description: Pet food dispenser based on STM HAL layer and FreeRTOS.
 * Gihub account: //todo: put the link here
 **************************************************************************/

#include "appConfig.h"
#include "buzzer.h"
#include "display.h"
#include "console.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"

TaskHandle_t xTaskHeartBeatHandler;
extern TaskHandle_t xTaskDisplayHandler;
extern void vTaskDisplay(void *params);

/*
*  Initialize the system clocks and clocks derived.
*/
void clkInit(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* Initializes the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/*
* Function to initialize the heart beat low level settings.
*/
static void heartBeatInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Heart beat: GPIO settings  */
    GPIO_InitStruct.Pin = HEART_BEAT_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(HEART_BEAT_LED_PORT, &GPIO_InitStruct);
}


/*
* Callback to increment the timer for the STM HAL layer.
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *timerHandler)
{
    if (timerHandler->Instance == TIM6)
    {
        HAL_IncTick();
    }
}

/*
* General handler for all the errors. TODO: Handle individual errors.
*/
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
        HAL_GPIO_WritePin(HEART_BEAT_LED_PORT, HEART_BEAT_LED_PIN, GPIO_PIN_RESET);
    }
}

/*
* Task to indicate the freeRTOS app is alive.
*/
void vTaskHeartBeat(void *params)
{
    while (1)
    {
        HAL_GPIO_TogglePin(HEART_BEAT_LED_PORT, HEART_BEAT_LED_PIN);
        vTaskDelay(pdMS_TO_TICKS(DELAY_HEART_BEAT_TASK));
    }
}

int main(void)
{
    BaseType_t retVal;
    HAL_StatusTypeDef halStatus;
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

    HAL_Init();
    /* Configure the system clock */
    //    clkInit(); // system CLK = 16 MHz
    /* Initialize heart beat led */
    //    heartBeatInit();
    /* Initialize debug console*/
    //    consoleInit();
    /* Initialize the display */
    //    displayInit();
    /* Initialize the buzzer */
    halStatus = buzzerInit();
    if (halStatus != HAL_OK)
    {
        Error_Handler();
    }

    /* Heart beat task */
    retVal = xTaskCreate(vTaskHeartBeat, "task-heart-beat", configMINIMAL_STACK_SIZE, NULL, HEART_BEAT_PRIORITY_TASK, &xTaskHeartBeatHandler);
    if (retVal != pdPASS)
    {
        goto main_out;
    }

    /* Display task */
    retVal = xTaskCreate(vTaskDisplay, "task-display", configMINIMAL_STACK_SIZE, NULL, DISPLAY_PRIORITY_TASK, &xTaskDisplayHandler);
    if (retVal != pdPASS)
    {
        goto main_out;
    }

    vTaskStartScheduler();

main_out:
    if (xTaskHeartBeatHandler != NULL)
    {
        vTaskDelete(xTaskHeartBeatHandler);
    }
    if (xTaskDisplayHandler != NULL)
    {
        vTaskDelete(xTaskDisplayHandler);
    }
    Error_Handler();
}
