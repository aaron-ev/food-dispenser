/**
 ******************************************************************************
 * @file         main.c
 * @author       Aaron Escoboza
 * @brief        Pet food dispenser based on STM HAL layer and FreeRTOS.
 *               Github account: https://github.com/aaron-ev
 ******************************************************************************
 */

#include "appConfig.h"
#include "main.h"
#include "display.h"
#include "bsp.h"
#include "console.h"
#include "stdlib.h"
#include "string.h"

/* 1 - Enable BSP testing, 0 - Disable BSP testing */
#define TEST_BSP                        (0)

/* Screen position for a string when feeding */
#define APP_POS_FEEDING_X               60
#define APP_POS_FEEDING_Y               210

/* Beep macros */
#define APP_BEEP_DELAY                  100

/* Global variables */
DispenserSettings dispenserSettings;
TaskHandle_t xTaskHeartBeatHandler;
extern TaskHandle_t xTaskRTCHandler;
extern TaskHandle_t xTaskDisplayHandler;

/* Extern task functions */
extern void vTaskDisplay(void *params);
extern void vTaskRTC(void *params);
/*
* Callback to increment the timer for the STM HAL layer.
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *timerHandler)
{
    if (timerHandler->Instance == TIM9)
    {
        HAL_IncTick();
    }
}

/*
* General handler for all the errors. TODO: Handle individual errors.
*/
void appErrorHandler(void)
{
    __disable_irq();
    HAL_GPIO_WritePin(HEART_BEAT_LED_PORT, HEART_BEAT_LED_PIN, GPIO_PIN_RESET);
    while (1)
    {
        PRINT_DEBUG("ERROR: Error handler\n");
        HAL_Delay(1000);
    }
}

/*
* Task to indicate the freeRTOS app is alive.
*/
void vTaskHeartBeat(void *params)
{
    while (1)
    {
        HAL_GPIO_TogglePin(HEART_BEAT_LED_PORT, HEART_BEAT_LED_PIN);
        vTaskDelay(pdMS_TO_TICKS(HEART_BEAT_BLINK_DELAY));
    }
}

/*
* Plays a beep.
*/
void appBeep(uint32_t numTimes)
{
    int i;

    if (numTimes <= 0 || dispenserSettings.sound == SOUND_OFF)
    {
        return;
    }
    for ( i = 0; i < numTimes; i++)
    {
        buzzerStart();
        HAL_Delay(APP_BEEP_DELAY);
        buzzerStop();
        if (numTimes > 1)
        {
            HAL_Delay(APP_BEEP_DELAY);
        }
    }
}

/*
* Test the buzzer feature.
*/
static void appTestBspBuzzer(void)
{
    /* Play different use cases */
    appBeep(4);
    appBeep(3);
    appBeep(2);
    appBeep(1);
}

/*
* Rotate the servo motor.
*/
void appServoRotate(Degrees_E degrees, uint32_t delay)
{
    servoMotorSetDegrees(degrees);
    servoMotorStart();
    vTaskDelay(pdMS_TO_TICKS(delay));
    servoMotorStop();
}

/*
* Test the servo motor: Iterate through all degrees
*/
static void appTestBspMotor(void)
{
    int i;
    for (i = 0; i < 30; i++)
    {
    	appFeed(5);
    }
}

/*
* Test the console feature.
*/
static void appTestConsole(void)
{
    int i;

		for (i= 0; i < 5; i++)
		{
			consolePrint("Hello world!\n");
		}
    }

void printTime(Time_s *time)
{
    printf("Time: \n"
            "day: %d\n"
            "year: %d\n"
            "month: %d\n"
            "date: %d\n"
            "hour: %d\n"
            "min: %d\n"
            "sec: %d\n"
            ,time->day,
             time->year,
             time->month,
             time->date,
             time->hour,
             time->min,
             time->sec
        );
}

void appTestRTC(void)
{

    uint8_t i;
    Time_s time = {0};
    Time_s newTime = {0};
    ClockPeriod currentClockPeriod;
    ClockSystem currentClockSystem;

    /********************** Testing seconds **********************/
    /* Test seconds: valid range */
    printf("RTC test: testing valid range (0 - 59) of seconds\n");
    for (i = 0; i < 59; i++)
    {
        time.sec = i;
        ds1302_setTime(&time);
        ds1302_get_time(&newTime);
        if (newTime.sec != time.sec)
        {
        	printf("ERROR: read %d written %d\n", newTime.sec, time.sec);
        	goto rtc_test_error;
        }
    }
    /********************** Testing minutes **********************/
    printf("RTC test: testing valid range (0 - 59) of minutes\n");
    for (i = 0; i < 59; i++)
    {
        time.min = i;
        ds1302_setTime(&time);
        ds1302_get_time(&newTime);
        if (newTime.min != time.min)
        {
        	printf("ERROR: read %d written %d\n", newTime.min, time.min);
        	goto rtc_test_error;
        }
    }
    /********************** Testing days **********************/
    printf("RTC test: testing valid range (1 - 7) of days\n");
    for (i = 1; i <  8; i++)
    {
        time.day = i;
        ds1302_setTime(&time);
        ds1302_get_time(&newTime);
        if (newTime.day != time.day)
        {
        	printf("ERROR: read %d written %d\n", newTime.day, time.day);
        	goto rtc_test_error;
        }
    }
    /********************** Testing years **********************/
    printf("RTC test: testing valid range (0 - 99) of years\n");
    for (i = 0; i < 100; i++)
    {
        time.year = i;
        ds1302_setTime(&time);
        ds1302_get_time(&newTime);
        if (newTime.year != time.year)
        {
        	printf("ERROR: read %d written %d\n", newTime.year, time.year);
        	goto rtc_test_error;
        }
    }
    /********************** Testing months **********************/
    printf("RTC test: testing valid range (1 - 12) of months\n");
    for (i = 1; i < 13; i++)
    {
        time.month = i;
        ds1302_setTime(&time);
        ds1302_get_time(&newTime);
        if (newTime.month != time.month)
        {
        	printf("ERROR: read %d written %d\n", newTime.month, time.month);
        	goto rtc_test_error;
        }
    }
    /********************** Testing dates **********************/
    printf("RTC test: testing valid range (1 - 31) of dates\n");
    for (i = 1; i < 32; i++)
    {
        time.date = i;
        ds1302_setTime(&time);
        ds1302_get_time(&newTime);
        if (newTime.date != time.date)
        {
        	printf("ERROR: read %d written %d\n", newTime.date, time.date);
        	goto rtc_test_error;
        }
    }
    /********************** Testing hours 1-12 clock system **********************/
    printf("RTC test: testing valid range (1 - 12) of hours\n");
    time.clockSystem = CLOCK_SYSTEM_12;
    for (i = 1; i < 13; i++)
    {
        time.hour = i;
        ds1302_setTime(&time);
        ds1302_get_time(&newTime);
        if (newTime.hour != time.hour)
        {
            printf("ERROR: read %d written %d\n", newTime.hour, time.hour);
            goto rtc_test_error;
        }
    }
    /********************** Testing hours 0 - 23 clock system **********************/
    printf("RTC test: testing valid range (0 - 23) of hours\n");
    time.clockSystem = CLOCK_SYSTEM_24;
    for (i = 0; i < 24; i++)
    {
        time.hour = i;
        ds1302_setTime(&time);
        ds1302_get_time(&newTime);
        if (newTime.hour != time.hour)
        {
            printf("ERROR: read %d written %d\n", newTime.hour, time.hour);
            goto rtc_test_error;
        }
    }

    /********************** Testing period and system clock **********************/
    printf("RTC test: testing period and system clock\n");

    /* Test clock system 12 and PM period */
    printf("RTC test: testing clock system 12 and PM period\n");
    time.clockSystem = CLOCK_SYSTEM_12;
    time.clockPeriod = CLOCK_PM_PERIOD;
    time.hour = 1;
    ds1302_setTime(&time);
    currentClockPeriod = ds1302_getClockPeriod();
    if (currentClockPeriod != time.clockPeriod)
    {
        printf("ERROR: clock period expected: %d, current: %d\n", time.clockPeriod, currentClockPeriod);
        goto rtc_test_error;
    }

    currentClockSystem = ds1302_getClockSystem();
    if (currentClockSystem != time.clockSystem)
    {
        printf("ERROR: clock system expected: %d, current: %d\n", time.clockSystem, currentClockSystem);
        goto rtc_test_error;
    }

    /* Test clock system 12 and AM period */
    printf("RTC test: testing clock system 12 and AM period\n");
    time.clockSystem = CLOCK_SYSTEM_12;
    time.clockPeriod = CLOCK_AM_PERIOD;
    time.hour = 1;
    ds1302_setTime(&time);

    currentClockPeriod = ds1302_getClockPeriod();
    if (currentClockPeriod != time.clockPeriod)
    {
        printf("ERROR: clock period expected: %d, current: %d\n", time.clockPeriod, currentClockPeriod);
        goto rtc_test_error;
    }

    currentClockSystem = ds1302_getClockSystem();
    if (currentClockSystem != time.clockSystem)
    {
        printf("ERROR: clock system expected: %d, current: %d\n", time.clockSystem, currentClockSystem);
        goto rtc_test_error;
    }

    /* Test clock system 24 */
    printf("RTC test: testing clock system 24\n");
    time.clockSystem = CLOCK_SYSTEM_24;
    time.hour = 0;
    ds1302_setTime(&time);
    currentClockSystem = ds1302_getClockSystem();
    if (currentClockSystem != time.clockSystem)
    {
        printf("ERROR: clock system expected: %d, current: %d\n", time.clockSystem, currentClockSystem);
        goto rtc_test_error;
    }

    /* Test ram */
    printf("RTC test: testing ram write and read \n");
    uint8_t tmpRamVal;
    for	(i = 0; i < 31; i++)
    {
        ds1302_write_ram(i, i);
         tmpRamVal = ds1302_read_ram(i);
        if (tmpRamVal != i)
        {
            printf("ERROR: RAM wrom value, expected %d read %d\n", i, tmpRamVal);
            goto rtc_test_error;
        }
    }
    printf("RTC test: testing ram clear operation \n");
    ds1302_clear_ram();
    for	(i = 0; i < 31; i++)
    {
        tmpRamVal = ds1302_read_ram(i);
        if (tmpRamVal != 0)
        {
            printf("ERROR: RAM wrom value, expected %d read %d\n", 0, tmpRamVal);
            goto rtc_test_error;
        }
    }

    printf("RTC TEST PASSED\n");
    return;

rtc_test_error:
	printf("RTC TEST FAIALED\n");
}

/*
* Test the BSP layer.
*/
static void appTestBsp(void)
{
    consolePrint("Testing: Console\n");
    appTestConsole();
    consolePrint("Testing: Buzzer\n");
    appTestBspBuzzer();
    consolePrint("Testing: Servo motor\n");
    appTestBspMotor();
    appTestRTC();
}

/*
* Feed the pet based on the portions.
*/
void appFeed(uint8_t portions)
{
    int i;
    char buff[15];

    if ((portions == 0) || (portions > DISPENSER_MAX_PORTIONS))
    {
        return;
    }

    consolePrint("APP: Feed started\n");
    mspDisableButtonIT();

    for (i = 0; i < portions; i++)
    {
        sprintf(buff, "Portion (%d)", i + 1);
        dispPrint(APP_POS_FEEDING_X, APP_POS_FEEDING_Y, buff,
                  Font_11x18, BLACK, WHITE);
        appServoRotate(SERVO_MOTOR_DEGREES_180, 250);
        vTaskDelay(pdMS_TO_TICKS(500));
        appServoRotate(SERVO_MOTOR_DEGREES_0, 250);
        vTaskDelay(pdMS_TO_TICKS(500));
        appBeep(1);
    }

    /* Clean the message on the screen */
    dispFillRect(APP_POS_FEEDING_X, APP_POS_FEEDING_Y, 200, 20, WHITE);
    mspEnableButtonIT();
    consolePrint("APP: Feed finished\n");
}

int main(void)
{
    BaseType_t retVal;
    HAL_StatusTypeDef halStatus;

    halStatus = bspInit();
    /* Initialize BSP layer */
    if (halStatus != HAL_OK)
    {
        consolePrint("APP: BSP could not be initialized\n");
        appErrorHandler();
    }
    consolePrint("BSP: Initialized\n");

    /* Initialize the display */
    displayInit();
    consolePrint("APP: Display Initialized\n");

    /* Initialize default dispenser settings */
    dispenserSettings.portions = 5;
    dispenserSettings.sound = DISPENSER_SOUND_ON;

    /* Double beep to indicate initialization phase is completed */
    mspEnableBuzzerIT();
    appBeep(2);

    /* Test mode: Check each feature functionality */
    #if (TEST_BSP == 1)
        consolePrint(" *** Testing mode *** \n");
        appTestBsp();
        consolePrint("Testing: Finished\n");
        exit(0);
    #endif

    /* Create tasks and start the scheduler */
    retVal = xTaskCreate(vTaskHeartBeat,
                         "task-heart-beat",
                         configMINIMAL_STACK_SIZE,
                         NULL,
                         HEART_BEAT_PRIORITY_TASK,
                         &xTaskHeartBeatHandler);
    if (retVal != pdTRUE)
    {
        consolePrint("ERROR: Heart beat task could not be created\n");
        goto main_out;
    }
    retVal = xTaskCreate(vTaskDisplay,
                         "task-display",
                         configMINIMAL_STACK_SIZE,
                         NULL,
                         DISP_PRIORITY_TASK,
                         &xTaskDisplayHandler);
    if (retVal != pdPASS)
    {
        consolePrint("ERROR: Display task could not be created\n");
        goto main_out;
    }
    retVal = xTaskCreate(vTaskRTC,
                         "task-rtc",
                         configMINIMAL_STACK_SIZE,
                         NULL,
                         RTC_PRIORITY_TASK,
                         &xTaskRTCHandler);
    if (retVal != pdPASS)
    {
        consolePrint("ERROR: RTC task could not be created\n");
        goto main_out;
    }

    vTaskStartScheduler();

main_out:
    consolePrint("ERROR: Not enough memory\n");
    if (!xTaskHeartBeatHandler)
    {
        vTaskDelete(xTaskHeartBeatHandler);
    }
    if (!xTaskDisplayHandler)
    {
        vTaskDelete(xTaskDisplayHandler);
    }
    if (!xTaskRTCHandler)
    {
        vTaskDelete(xTaskDisplayHandler);
    }
    appErrorHandler();
}
