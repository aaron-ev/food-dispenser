/**
  ******************************************************************************
  * @file    appConfig.h
  * @author  Aaron Escoboza
  * @brief   Hold general application configuration
  ******************************************************************************
*/

#ifndef APP_CONFIG__H
#define APP_CONFIG__H

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

/* Task priorities */
#define HEART_BEAT_PRIORITY_TASK           1
#define DISPLAY_PRIORITY_TASK              2

/* Heart beat settings */
#define HEART_BEAT_LED_PORT                 GPIOC
#define HEART_BEAT_LED_PIN                  GPIO_PIN_13
#define HEART_BEAT_BLINK_DELAY              500 /* In ms */

/* Buzzer settings */
#define BUZZER_GPIO_INSTANCE                GPIOA
#define BUZZER_GPIO_PIN                     GPIO_PIN_0
#define BUZZER_GPIO_ALTERNATE               GPIO_AF1_TIM2
#define BUZZER_TIM_INSTANCE                 TIM2
#define BUZZER_TIM_CHANNEL                  TIM_CHANNEL_1
#define BUZZER_TIM_PRESCALER                40  /* tick = 1us*/
#define BUZZER_TIM_PULSE_VALUE              500 /*freq = 1khz, 1us * 500 * 2 */

/* Servo motor settings */
#define SERVO_MOTOR_GPIO_INSTANCE           GPIOA
#define SERVO_MOTOR_GPIO_PIN                GPIO_PIN_1
#define SERVO_MOTOR_GPIO_ALTERNATE          GPIO_AF2_TIM5
#define SERVO_MOTOR_TIM_INSTANCE            TIM5
#define SERVO_MOTOR_TIM_CHANNEL             TIM_CHANNEL_2

/* Push buttons setting */
#define BUTTON_ENTER_GPIO_PORT              GPIOA
#define BUTTON_ENTER_GPIO_PIN               GPIO_PIN_2
#define BUTTON_UP_GPIO_PORT                 GPIOA
#define BUTTON_UP_GPIO_PIN                  GPIO_PIN_3
#define BUTTON_DOWN_GPIO_PORT               GPIOA
#define BUTTON_DOWN_GPIO_PIN                GPIO_PIN_4

/* Backlight settings */
#define DISPLAY_BACKLIGHT_DEFAULT_PERIOD    100000 /* 1m as a default */
/* Dispenser settings */
#define DISPENSER_MAX_PORTIONS              5
#endif /* APP_CONFIG__H */
