/**
  ******************************************************************************
  * @file    appConfig.h
  * @author  Aaron Escoboza
  * @brief   Hold general application configuration
  ******************************************************************************
*/

#ifndef APP_CONFIG__H
#define APP_CONFIG__H

#include "stm32f4xx_hal.h"

/* Private defines -----------------------------------------------------------*/
#define B1_Pin                             GPIO_PIN_13
#define B1_GPIO_Port                       GPIOC
#define USART_TX_Pin                       GPIO_PIN_2
#define USART_TX_GPIO_Port                 GPIOA
#define USART_RX_Pin                       GPIO_PIN_3
#define USART_RX_GPIO_Port                 GPIOA

#define LD2_Pin                            GPIO_PIN_5
#define LD2_GPIO_Port                      GPIOA

#define TMS_Pin                            GPIO_PIN_13
#define TMS_GPIO_Port                      GPIOA
#define TCK_Pin                            GPIO_PIN_14
#define TCK_GPIO_Port                      GPIOA

#define SWO_Pin                            GPIO_PIN_3
#define SWO_GPIO_Port                      GPIOB

/* Project version */
#define DISPENSER_VERSION_MAYOR_NUMBER     0
#define DISPENSER_VERSION_MINOR_NUMBER     1

/* Task priorities */
#define HEART_BEAT_PRIORITY_TASK           1
#define DISPLAY_PRIORITY_TASK              2

/* Delays*/
#define DELAY_HEART_BEAT_TASK               1000 /* In ms */

/* Heart beat settings */
#define HEART_BEAT_LED_PORT                 GPIOA
#define HEART_BEAT_LED_PIN                  GPIO_PIN_5

/* Buzzer settings */
#define BUZZER_GPIO_INSTANCE                GPIOA
#define BUZZER_GPIO_PIN                     GPIO_PIN_0
#define BUZZER_GPIO_ALTERNATE               GPIO_AF1_TIM2
#define BUZZER_TIM_INSTANCE                 TIM2
#define BUZZER_TIM_CHANNEL                  TIM_CHANNEL_1
#define BUZZER_TIM_PRESCALER                15           /* tick = 1us*/
#define BUZZER_TIM_PULSE_VALUE              500          /*freq = 1khz, 1us * 500 * 2 */

#endif /* APP_CONFIG__H */
