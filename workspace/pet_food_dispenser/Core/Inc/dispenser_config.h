/*
 * dispenser_config.h
 *
 *  Created on: Nov 12, 2022
 *      Author: Aaron Escoboza
 */

#ifndef INC_DISPENSER_CONFIG_H_
#define INC_DISPENSER_CONFIG_H_
#include "main.h"


/* Task priorities */
#define HEART_BEAT_PRIORITY_TASK        1

/* Delays*/
#define DELAY_HEART_BEAT_TASK   500 /* In ms */

/* GPIO definitions */
#define HEART_BEAT_LED_PORT     GPIOA
#define HEART_BEAT_LED_PIN      GPIO_PIN_5

#endif /* INC_DISPENSER_CONFIG_H_ */
