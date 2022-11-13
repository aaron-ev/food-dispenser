#include "dispenser_config.h"
#include "freeRTOS.h"
#include "task.h"
#include "ili9341.h"
#include <stdio.h>

/****************************************************************************
*                      Global variables
*****************************************************************************/
UART_HandleTypeDef huart2;
SPI_HandleTypeDef hspi1;
TaskHandle_t xTaskHeartBeatHandler;
TaskHandle_t xTaskDisplayHandler;

/****************************************************************************
*                       Function prototypes
*****************************************************************************/
void systemClockConfig(void);
static void gpioInit(void);
static void uartInit(void);
static void displayInit(void);

/****************************************************************************
*                       Function definitions
*****************************************************************************/
void systemClockConfig(void)
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

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;   
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
      Error_Handler();
    }
}

static void uartInit(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
      Error_Handler();
    }
}

static void gpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /*Configure GPIO pin : Heart beat led */
    GPIO_InitStruct.Pin = HEART_BEAT_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(HEART_BEAT_LED_PORT, &GPIO_InitStruct);
}

static void displayInit(void)
{
    ILI9341_Init();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

void vTaskHeartBeat(void *params)
{
    while (1)
    {
        HAL_GPIO_TogglePin(HEART_BEAT_LED_PORT, HEART_BEAT_LED_PIN);
        vTaskDelay(pdMS_TO_TICKS(DELAY_HEART_BEAT_TASK));
    }
}

void vTaskDisplay(void *params)
{
    while (1)
    {  
        //TODO: 1. block until there is a new touch point 
        //      2. implement a FSM to handle the menu 
    }
}


int main(void)
{
	BaseType_t retVal;
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    /* Configure the system clock */
    systemClockConfig();
    /* Initialize all configured peripherals */
    gpioInit();
    /* Initialize UART for debugging purposes*/
    uartInit();
    /* Initialize Display */
    displayInit();

    /* Create tasks */
    retVal = xTaskCreate(vTaskHeartBeat, "task-heart-beat", configMINIMAL_STACK_SIZE, NULL, 1, &xTaskHeartBeatHandler);
    if (retVal != pdPASS)
    {
        printf("Error: insufficient memory for heart beat task\n");
        goto main_out;
    }

    retVal = xTaskCreate(vTaskDisplay, "task-display", configMINIMAL_STACK_SIZE, NULL, 2, &xTaskDisplayHandler);
    if (retVal != pdPASS)
    {
        printf("Error: insufficient memory for display task\n");
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
  HAL_GPIO_WritePin(HEART_BEAT_LED_PORT, HEART_BEAT_LED_PIN, GPIO_PIN_SET);
  return 1;
}
