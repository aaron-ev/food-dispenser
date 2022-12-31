/**
 ******************************************************************************
 * @file         msp.c
 * @author       Aaron Escoboza
 * @brief        Low level initialization: GPIO, CLKs, NVIC
 ******************************************************************************
 */

#include "stm32f4xx_hal.h"
#include "appConfig.h"
#include "main.h"
#include "msp.h"

/*
 *   Enable clocks and set NVIC settings.
 */
void HAL_MspInit(void)
{
    /* Enable clocks for some system settings */
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    /* Enable clock for GPIOs being used */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /*Enable clock for TIMERS being used */
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_TIM5_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();
    /* Enable clock for the dconsole */
    __HAL_RCC_USART1_CLK_ENABLE();
    /* Set NVIC priority configuration */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    HAL_NVIC_SetPriority(EXTI2_IRQn, 15, 0);
    HAL_NVIC_SetPriority(EXTI3_IRQn, 15, 0);
    HAL_NVIC_SetPriority(EXTI4_IRQn, 15, 0);
}

/*
 *   GPIO settings: Buzzer.
 */
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *timerHandler)
{
    GPIO_InitTypeDef buzzerGpioInit = {0};

    /* BUZZER: GPIO settings */
    buzzerGpioInit.Pin = BUZZER_GPIO_PIN;
    buzzerGpioInit.Mode = GPIO_MODE_AF_PP;
    buzzerGpioInit.Pull = GPIO_NOPULL;
    buzzerGpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    buzzerGpioInit.Alternate = BUZZER_GPIO_ALTERNATE;
    HAL_GPIO_Init(BUZZER_GPIO_INSTANCE, &buzzerGpioInit);
}

/*
 *   GPIO settings: Servo motor.
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *timerHandler)
{
    GPIO_InitTypeDef servoMotorGpioInit = {0};

    /* GPIO settings: ServoMotor */
    servoMotorGpioInit.Pin = SERVO_MOTOR_GPIO_PIN;
    servoMotorGpioInit.Mode = GPIO_MODE_AF_PP;
    servoMotorGpioInit.Alternate = SERVO_MOTOR_GPIO_ALTERNATE;
    HAL_GPIO_Init(SERVO_MOTOR_GPIO_INSTANCE, &servoMotorGpioInit);
}

void mspEnableButtonInterrupts(void)
{
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    PRINT_DEBUG("Buttons: Interrupts enabled\n");
}

void mspEnableBuzzerInterrupts(void)
{
    HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void mspDisableButtonInterrupts(void)
{
    HAL_NVIC_DisableIRQ(EXTI2_IRQn);
    HAL_NVIC_DisableIRQ(EXTI3_IRQn);
    HAL_NVIC_DisableIRQ(EXTI4_IRQn);
    PRINT_DEBUG("Buttons: Interrupts disabled\n");
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandler)
{
    GPIO_InitTypeDef uartGpioInit;

    if (uartHandler->Instance == CONSOLE_INSTANCE)
    {
        uartGpioInit.Pin = CONSOLE_TX_PIN | CONSOLE_RX_PIN;
        uartGpioInit.Mode = GPIO_MODE_AF_PP;
        uartGpioInit.Pull = GPIO_PULLUP;
        uartGpioInit.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        uartGpioInit.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(CONSOLE_GPIO_PORT, &uartGpioInit);
    }
}
