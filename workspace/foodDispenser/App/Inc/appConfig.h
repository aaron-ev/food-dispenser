/**
  ******************************************************************************
  * @file    appConfig.h
  * @author  Aaron Escoboza
  * @brief   Hold general application configuration
  ******************************************************************************
*/

#ifndef APP_CONFIG__H
#define APP_CONFIG__H

/* Enable debug messages */
#define DEBUG_PRINT_EN                      0 /* 1 = Enable , 0 =  Disable */

/* Dispenser settings */
#define DISPENSER_MAX_PORTIONS              5

/* Task priorities */
#define HEART_BEAT_PRIORITY_TASK            1
#define DISP_PRIORITY_TASK                  2

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
#define BEEP_DEFAULT_TON                    100
#define BEEP_DEFAULT_TOFF                   BEEP_DEFAULT_TON

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
#define BUTTON_DEBOUNCE_DELAY               30

/* Backlight settings */
#define DISP_BACKLIGHT_DEFAULT_PERIOD       100000 /* 1m as a default settings*/

/* Debug console settings */
#define CONSOLE_INSTANCE                    USART1
#define CONSOLE_TX_PIN                      GPIO_PIN_9
#define CONSOLE_RX_PIN                      GPIO_PIN_10
#define CONSOLE_GPIO_PORT                   GPIOA
#define CONSOLE_BAUDRATE                    9600

#endif
