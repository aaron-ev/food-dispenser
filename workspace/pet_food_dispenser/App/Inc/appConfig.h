#ifndef APP_CONFIG__H
#define APP_CONFIG__H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
#define B1_Pin                      GPIO_PIN_13
#define B1_GPIO_Port                GPIOC
#define USART_TX_Pin                GPIO_PIN_2
#define USART_TX_GPIO_Port          GPIOA
#define USART_RX_Pin                GPIO_PIN_3
#define USART_RX_GPIO_Port          GPIOA

#define LD2_Pin                     GPIO_PIN_5
#define LD2_GPIO_Port               GPIOA

#define TMS_Pin                     GPIO_PIN_13
#define TMS_GPIO_Port               GPIOA
#define TCK_Pin                     GPIO_PIN_14
#define TCK_GPIO_Port               GPIOA

#define SWO_Pin                     GPIO_PIN_3
#define SWO_GPIO_Port               GPIOB

/* Project version */
#define DISPENSER_VERSION_MAYOR_NUMBER     0
#define DISPENSER_VERSION_MINOR_NUMBER     1

/* Task priorities */
#define HEART_BEAT_PRIORITY_TASK        1

/* Delays*/
#define DELAY_HEART_BEAT_TASK   1000 /* In ms */

/* GPIO definitions */
#define HEART_BEAT_LED_PORT     GPIOA
#define HEART_BEAT_LED_PIN      GPIO_PIN_5

/* Buzzer settings */
#define BUZZER_GPIO_INSTANCE       GPIOA
#define BUZZER_GPIO_PIN            GPIO_PIN_10
#define BUZZER_TIM_INSTANCE        TIM2
#define BUZZER_TIM_CHANNEL         TIM_CHANNEL_1

#ifdef __cplusplus
}
#endif

#endif /* APP_CONFIG__H */
