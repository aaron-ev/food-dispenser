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

extern UART_HandleTypeDef consoleHandle;

void consolePrint(const char *pData)
{
    HAL_UART_Transmit(&consoleHandle, (uint8_t *)pData, strlen(pData), HAL_MAX_DELAY);
}
