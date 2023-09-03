

#include "bsp.h"
#include "bluetooth.h"

UART_HandleTypeDef uartHandler;

void bluetoothInit(void)
{

    uartHandler.Instance = BLU2TH_UART_INSTANCE; 
    uartHandler.Init.Baudrate = BLU2TH_UART_BAUDRATE; 
    uartHandler.Init.WordLength  = UART_WORDLENGTH_8B;
    uartHandler.Init.StopBits = UART_STOPBITS_1;
    uartHandler.Init.Parity = UART_PARITY_NONE;
    uartHandler.Init.Mode = UART_MODE_TX_RX;
    uartHandler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uartHandler.Init.OverSampling = UART_OVERSAMPLING_16;
    uartHandler.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLED; 
    if (HAL_UART_Init(&uartHandler) != HAL_OK)
    {
        //TODO
    }
}
