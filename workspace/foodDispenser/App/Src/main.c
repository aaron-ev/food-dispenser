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
extern TaskHandle_t xTaskDisplayHandler;

/* Extern task functions */
extern void vTaskDisplay(void *params);

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

    /* Initialize BSP layer */
    halStatus = bspInit();
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
    appErrorHandler();
}
