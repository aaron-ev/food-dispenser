#include "main.h"

extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef timHandler;
/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim6);
}

void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&timHandler);
}
