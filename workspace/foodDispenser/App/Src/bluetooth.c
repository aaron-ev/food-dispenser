
#include "bsp.h"
#include "queue.h"

#define QUEUE_SIZE                100

char charReceive = 0;
UART_HandleTypeDef uartHandler;
QueueHandle_t serialQueueHandle;
TaskHandle_t xTaskBluetoothHandler;
extern DispenserSettings dispenserSettings;

typedef enum
{
    CMD_FEED,
    CMD_BEEP,
    CMD_SOUND,
}Command;

typedef struct
{
    Command cmd;
    uint8_t data;
}Package;

void blueToothResponse(char *str)
{
    size_t len = strlen(str);
    if (len > 1)
    {
        HAL_UART_Transmit_IT(&uartHandler, str, len);
    }
}
void vTaskBluetooth(void *parms)
{
    Package rxPackage;
    char charRead = 0;
    int counterChar = 0;

    /* Create queue to handle uart reception */
    serialQueueHandle  = xQueueCreate(QUEUE_SIZE, sizeof(char));
    if (serialQueueHandle  == NULL)
    {
        printf("Bluetooth: Queue could not created\n");
        vTaskDelete(NULL);
    }

    HAL_UART_Receive_IT(&uartHandler, &charReceive, 1);
    while (1)
    {
        while(charRead != '\n')
        {
            xQueueReceive(serialQueueHandle , &charRead, portMAX_DELAY);
            /* pos 0 = cmd, pos 1 = blank space, pos 2 = data */
            if (counterChar == 0)
            {
                rxPackage.cmd = charRead - 48;
            }
            else if (counterChar == 2)
            {
                rxPackage.data = charRead - 48;
            }
            else 
            {
                /* Do nothing: /r/n characters */
            }
            counterChar++;
        }
        counterChar = 0;
        charRead = 0;
        processCommand(rxPackage.cmd, rxPackage.data);
        // printf("cmd: %d\n", rxPackage.cmd);
        // printf("data: %d\n", rxPackage.data);
    }
}

void processCommand(Command cmd, uint8_t data)
{
    switch (cmd)
    {
        case CMD_FEED:
        {
            if ((data >= 1) && (data <= 5))
            {
                appFeed(data);
            }
            else
            {
            	goto invalid_cmd;
            }
            break;
        }
        case CMD_BEEP:
        {
            if ((data >= 1) && (data <= 5))
            {
                appBeep(data);
            }
            else
            {
            	goto invalid_cmd;
            }
            break;
        }
        case CMD_SOUND:
        {
            if (data == DISPENSER_SOUND_ON)
            {
                dispenserSettings.sound = DISPENSER_SOUND_ON;
            }
            else if (data == SOUND_OFF)
            {
                dispenserSettings.sound = SOUND_OFF;
            }
            else
            {
                /* Invalid data */
                goto invalid_cmd;
            }
            break;
        }
        default: {goto invalid_cmd; break;}
    }
    blueToothResponse("ok");

invalid_cmd:
    blueToothResponse("noOK");
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t status;
    BaseType_t xHigherPriorityTaskWoken;

    status = xQueueSendToFrontFromISR(serialQueueHandle, &charReceive, &xHigherPriorityTaskWoken);
    if (status != pdPASS)
    {
        printf("Error: Could not set data to queue\n");
        blueToothResponse("noOK");
    }
    HAL_UART_Receive_IT(&uartHandler, &charReceive, 1);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    printf("TX callback\n");
}
