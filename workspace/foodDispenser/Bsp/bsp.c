/**
  ******************************************************************************
  * @file    bsp.c
  * @author  Aaron Escoboza
  * @brief   source file to implement init functions
  ******************************************************************************
*/

#include "bsp.h"

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
    RCC_OscInitStruct.PLL.PLLN = 80;
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

void pushButtonsInit(void)
{
    GPIO_InitTypeDef pushButtonsGpioInit = {0};

    /* Push buttons common settings */
    pushButtonsGpioInit.Mode = GPIO_MODE_IT_RISING;
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

HAL_StatusTypeDef bspInit(void)
{
    HAL_StatusTypeDef halStatus = HAL_OK;

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    /* Configure the system clock */
    clkInit(); // system CLK = 16 MHz
    /* Initialize push buttons */
    pushButtonsInit();
    /* Initialize heart beat led */
    heartBeatInit();
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

    return halStatus;
}