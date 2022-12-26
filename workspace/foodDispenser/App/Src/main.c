
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

#define TEST_BSP            (0)

TaskHandle_t xTaskHeartBeatHandler;
extern TaskHandle_t xTaskDisplayHandler;
extern void vTaskDisplay(void *params);
DispenserSettings dispenserSettings;

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
void errorHandler(void)
{
    __disable_irq();
    HAL_GPIO_WritePin(HEART_BEAT_LED_PORT, HEART_BEAT_LED_PIN, GPIO_PIN_RESET);
    while (1)
    {
        PRINT_DEBUG("Error handler\n");
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

void dispenserBeep(uint32_t timeOn, uint32_t timeOff, uint32_t times)
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

void testBspBuzzer(void)
{
    dispenserBeep(300, 300, 4);
    dispenserBeep(200, 200, 3);
    dispenserBeep(100, 100, 2);
    dispenserBeep(50, 0, 1);
}

void appServoRotate(ServoPosition position, uint32_t halDelay)
{
    servoMotorSetPosition(position);
    servoMotorStart();
    HAL_Delay(halDelay);
    servoMotorStop();
}

void testBspServoMotor(void)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        appServoRotate(SERVO_MOTOR_DEGREES_180, 500);
        appServoRotate(SERVO_MOTOR_DEGREES_0, 500);
    }
}

void testConsole()
{
    int i;

    for (i= 0; i < 5; i++)
    {
        consolePrint("Hello world!\n");
    }
}

void testBsp(void)
{
//   testBspBuzzer();
   consolePrint("Testing: Servo motor\n");
   testBspServoMotor();
//   testConsole();
}

int main(void)
{
    BaseType_t retVal;
    HAL_StatusTypeDef halStatus;

    /* Initialize low level settings */
    halStatus = bspInit();
    consolePrint("BSP initialized\n");
    if (halStatus != HAL_OK)
    {
        errorHandler();
    }
    /* Initialize display */
    displayInit();
    consolePrint("Display initialized\n");
    /* Initialize default dispenser settings */
    dispenserSettings.portions = 1;
    dispenserSettings.sound = DISPENSER_SOUND_ON;
    /* Double beep to indicate all the initializations have finished */
    mspEnableBuzzerInterrupts();

    /* Set servo motor to default position */
    appServoRotate(SERVO_MOTOR_DEGREES_0, 250);
    dispenserBeep(100, 100, 2);
    /* Test BSP layer */
    #if (TEST_BSP == 1)
        consolePrint(" *** Testing mode *** \n");
        testBsp();
        consolePrint("Testing: Finished\n");
        /* Finish the program execution */
        exit(0);
    #endif

    /* Heart beat task */
    retVal = xTaskCreate(vTaskHeartBeat, "task-heart-beat", configMINIMAL_STACK_SIZE, NULL, HEART_BEAT_PRIORITY_TASK, &xTaskHeartBeatHandler);
    if (retVal != pdTRUE)
    {
        goto main_out;
    }
    /* Display task */
    retVal = xTaskCreate(vTaskDisplay, "task-display", configMINIMAL_STACK_SIZE, NULL, DISPLAY_PRIORITY_TASK, &xTaskDisplayHandler);
    if (retVal != pdPASS)
    {
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
    errorHandler();
}
