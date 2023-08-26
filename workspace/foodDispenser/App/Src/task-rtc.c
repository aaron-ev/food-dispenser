#include "main.h"
#include "appConfig.h"
#include "bsp.h"
#include "console.h"
#include "DS1302/ds1302.h"

TaskHandle_t xTaskRTCHandler;

void vTaskRTC(void *params)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        consolePrint("Task RTC\n");
    }
}
