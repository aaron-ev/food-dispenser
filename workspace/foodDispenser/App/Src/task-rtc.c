#include "main.h"
#include "appConfig.h"
#include "bsp.h"
#include "console.h"
#include "DS1302/ds1302.h"

Time_s time;
TaskHandle_t xTaskRTCHandler;

void vTaskRTC(void *params)
{
    char buff[20];

    time.clockSystem = DS1302_CLK_SYSTEM_12;
    time.clockPeriod = DS1302_CLK_PM_PERIOD;
    time.hour = 12;
    time.min = 30;
    time.sec = 1; 
    ds1302_set_time(&time);

    while (1)
    {
        ds1302_get_time(&time);
        if (time.clockPeriod ==  DS1302_CLK_PM_PERIOD)
        {
        	sprintf(buff, "%2d:%2d:%2d%2s", time.hour, time.min, time.sec, "PM");
        }
        else
        {
        	sprintf(buff, "%2d:%2d:%2d%2s", time.hour, time.min, time.sec, "AM");
        }
        tft_ili9341_send_str(10, 20, buff, Font_11x18, BLACK, WHITE);
        vTaskDelay(pdMS_TO_TICKS(1000));
        consolePrint("Task RTC\n");
    }
}
