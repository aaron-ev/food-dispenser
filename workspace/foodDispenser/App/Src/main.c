
/**************************************************************************
 * Author: Aaron Escoboza
 * Description: Pet food dispenser based on STM HAL layer and FreeRTOS.
 * Github account: https://github.com/aaron-ev
 **************************************************************************/

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
void appBeep(uint32_t timeOn, uint32_t timeOff, uint32_t times)
{
    int i;

    if (times <= 0 || timeOn <= 0 || dispenserSettings.sound == DISPENSER_SOUND_OFF)
    {
        return;
    }
    for ( i = 0; i < times; i++)
    {

        buzzerStart();
        HAL_Delay(timeOn);
        buzzerStop();
        if (times > 1)
        {
            HAL_Delay(timeOff);
        }
    }
}

/*
* Test the buzzer feature.
*/
static void appTestBspBuzzer(void)
{
    appBeep(300, 300, 4);
    appBeep(200, 200, 3);
    appBeep(100, 100, 2);
    appBeep(50, 0, 1);
}

/*
* Rotate the servo motor.
*/
void appServoRotate(ServoPosition position, uint32_t halDelay)
{
    servoMotorSetPosition(position);
    servoMotorStart();
    HAL_Delay(halDelay);
    servoMotorStop();
}

/*
* Test the servo motor feature.
*/
static void appTestBspServoMotor(void)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        appServoRotate(SERVO_MOTOR_DEGREES_180, 500);
        appServoRotate(SERVO_MOTOR_DEGREES_0, 500);
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
* Test the bsp layer.
*/
static void testBsp(void)
{
  consolePrint("Testing: Console\n");
  appTestConsole();
  consolePrint("Testing: Buzzer\n");
  appTestBspBuzzer();
  consolePrint("Testing: Servo motor\n");
  appTestBspServoMotor();
}

/*
* Feed the pet based on the portions.
*/
void appFeed(uint8_t portions)
{
    int i;
    char buff[15];

    PRINT_DEBUG("APP: Feed started\n");
    mspDisableButtonInterrupts();
    if ((portions == 0) || (portions > DISPENSER_MAX_PORTIONS))
    {
        return;
    }

    /* Move the motor */
    for (i = 0; i < portions; i++)
    {
        sprintf(buff, "Portion (%d)", i + 1);
        dispPrint(APP_POS_FEEDING_X, APP_POS_FEEDING_Y, buff,
                  Font_11x18, BLACK, WHITE);
        appServoRotate(SERVO_MOTOR_DEGREES_180, 250);
        HAL_Delay(500);
        appServoRotate(SERVO_MOTOR_DEGREES_0, 250);
        HAL_Delay(500);
    }

    /* Clean the message on the screen */
    dispFillRect(APP_POS_FEEDING_X, APP_POS_FEEDING_Y, 200, 20, WHITE);
    mspEnableButtonInterrupts();
    PRINT_DEBUG("APP: Feed finished\n");
}

int main(void)
{
    BaseType_t retVal;
    HAL_StatusTypeDef halStatus;

    /* Initialize bsp layer */
    halStatus = bspInit();
    if (halStatus != HAL_OK)
    {
        appErrorHandler();
    }
    consolePrint("BSP: Initialized\n");

    /* Initialize display */
    dispInit();
    consolePrint("Display: Initialized\n");

    /* Test mode to check the bsp correct functionality */
    #if (TEST_BSP == 1)
        consolePrint(" *** Testing mode *** \n");
        testBsp();
        consolePrint("Testing: Finished\n");
        exit(0);
    #endif

    /* Initialize default dispenser settings */
    dispenserSettings.portions = 1;
    dispenserSettings.sound = DISPENSER_SOUND_ON;

    /* Set the servo motor to its default position */
    appServoRotate(SERVO_MOTOR_DEGREES_0, 250);

    /* Double beep to indicate initialization phase is completed */
    mspEnableBuzzerInterrupts();
    appBeep(100, 100, 2);

    /* Heart beat task */
    retVal = xTaskCreate(vTaskHeartBeat,
                         "task-heart-beat",
                         configMINIMAL_STACK_SIZE,
                         NULL,
                         HEART_BEAT_PRIORITY_TASK,
                         &xTaskHeartBeatHandler);
    if (retVal != pdTRUE)
    {
        consolePrint("ERROR: Heart beat could not be created\n");
        goto main_out;
    }

    /* Display task */
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
    if (xTaskHeartBeatHandler != NULL)
    {
        vTaskDelete(xTaskHeartBeatHandler);
    }
    if (xTaskDisplayHandler != NULL)
    {
        vTaskDelete(xTaskDisplayHandler);
    }
    appErrorHandler();
}
