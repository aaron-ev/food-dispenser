/**
 ******************************************************************************
 * @file         display.c
 * @author       Aaron Escoboza
 * @brief        Source file for the display feature
 ******************************************************************************
 */
#include "stm32f4xx.h"
#include "string.h"
#include "console.h"

#define CONSOLE_PRINTF int __io_putchar(int ch)

extern UART_HandleTypeDef consoleHandle;

void consolePrint(const char *pData)
{
    if (sizeof(pData) < 1)
    {
        return;
    }
    HAL_UART_Transmit(&consoleHandle, (uint8_t *)pData, strlen(pData), HAL_MAX_DELAY);
}

CONSOLE_PRINTF
{
	HAL_UART_Transmit(&consoleHandle, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
}
