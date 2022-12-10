
/**************************************************************************
 * Author: Aaron Escoboza
 * Description: Pet food dispenser based on STM HAL layer and FreeRTOS.
 * Github account: https://github.com/aaron-ev
 **************************************************************************/

#include "main.h"
#include "appConfig.h"
#include "buzzer.h"
#include "servoMotor.h"
#include "display.h"
#include "console.h"
#include "FreeRTOS.h"
#include "task.h"

#define TEST_BSP 1

TaskHandle_t xTaskHeartBeatHandler;
extern TaskHandle_t xTaskDisplayHandler;
extern void vTaskDisplay(void *params);
void errorHandler(void);
DispenserSettings dispenserSettings;

/*
*  Initialize the system clocks and clocks derived.
*/
void clkInit(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 64;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        errorHandler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        errorHandler();
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
    if (timerHandler->Instance == TIM9)
    {
        HAL_IncTick();
    }
}

/*
* General handler for all the errors. TODO: Handle individual errors.
*/
void errorHandler(void)
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
        vTaskDelay(pdMS_TO_TICKS(HEART_BEAT_BLINK_DELAY));
    }
}

void pushButtonsInit(void)
{
    GPIO_InitTypeDef pushButtonsGpioInit = {0};

    /* Push buttons common settings */
    pushButtonsGpioInit.Mode = GPIO_MODE_IT_FALLING;
    pushButtonsGpioInit.Pull = GPIO_NOPULL;
    pushButtonsGpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    /* Push button UP: GPIO settings */
    pushButtonsGpioInit.Pin = BUTTON_UP_GPIO_PIN;
    HAL_GPIO_Init(BUTTON_UP_GPIO_PORT, &pushButtonsGpioInit);
    /* Push button DOWN: GPIO settings */
    pushButtonsGpioInit.Pin = BUTTON_DOWN_GPIO_PIN;
    HAL_GPIO_Init(BUTTON_DOWN_GPIO_PORT, &pushButtonsGpioInit);
    /* Push button ENTER: GPIO settings */
    pushButtonsGpioInit.Pin = BUTTON_ENTER_GPIO_PIN;
    HAL_GPIO_Init(BUTTON_ENTER_GPIO_PORT, &pushButtonsGpioInit);

    /* ENVIC settings */
     HAL_NVIC_EnableIRQ(EXTI0_IRQn);
     HAL_NVIC_EnableIRQ(EXTI1_IRQn);
     HAL_NVIC_EnableIRQ(EXTI2_IRQn);
     HAL_NVIC_SetPriority(EXTI0_IRQn, 15, 0);
     HAL_NVIC_SetPriority(EXTI1_IRQn, 15, 0);
     HAL_NVIC_SetPriority(EXTI2_IRQn, 15, 0);
}

void dispenserBeep(uint32_t timeOn, uint32_t timeOff, uint32_t times)
{
    int i;

    if (times <= 0 || timeOn <= 0 || dispenserSettings.sound == DISPENSER_SOUND_OFF)
    {
        return;
    }

    for ( i = 0; i < times; i++)
    {

        buzzerStart();
        HAL_Delay(timeOn);
        buzzerStop();
        if (times > 1)
        {
            HAL_Delay(timeOff);
        }
    }
}

void testBspBuzzer(void)
{
	dispenserBeep(300, 300, 4);
	dispenserBeep(200, 200, 3);
	dispenserBeep(100, 100, 2);
    dispenserBeep(50, 0, 1);
}

void testBspServoMotor(void)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        servoMotorRotate(SERVO_MOTOR_DEGREES_180);
        dispenserBeep(100, 0, 1);
        HAL_Delay(250);
        servoMotorRotate(SERVO_MOTOR_DEGREES_0);
        dispenserBeep(100, 0, 1);
        HAL_Delay(250);
    }
}

void testBsp(void)
{
    testBspBuzzer();
    testBspServoMotor();
}

int main(void)
{
    BaseType_t retVal;
    HAL_StatusTypeDef halStatus;
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    /* Configure the system clock */
    // clkInit(); // system CLK = 16 MHz
    /* Initialize push buttons */
//    pushButtonsInit();
    /* Initialize heart beat led */
    heartBeatInit();
    /* Initialize debug console*/
    // consoleInit();
    /* Initialize display */
//    displayInit();
    /* Initialize servomotor */
    halStatus = servoMotorInit();
    if (halStatus != HAL_OK)
    {
        errorHandler();
    }
    /* Initialize the buzzer */
    halStatus = buzzerInit();
    if (halStatus != HAL_OK)
    {
        errorHandler();
    }
    /* Initialize default dispenser settings */
    dispenserSettings.portions = 1;
    dispenserSettings.sound = DISPENSER_SOUND_ON;
    /* Double beep to indicate all the initializations have finished */
    dispenserBeep(100, 100, 2);

    /* Test BSP layer */
    #if (TEST_BSP == 1)
        testBsp();
    #endif

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
    errorHandler();
}
