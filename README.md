# Pet food dispenser
This project is based on the STM32F401 microcontroller and freeRTOS,
it can be applied to any STM32 microcontroller that uses HAL layer with just
minimal changes.

The development board used to test it was the **Songhe STM32F401** which is
the previous version of the actual **black pill** development board so if you
have the black pill would be easy to implement the whole project.stm32

## Index
1. [Project features](#id1)
2. [Project architecture](#id2)
3. [App configuration](#id3)
3. [Use example](#id4)

## Project features<a name="id1"></a>
* Servomotor to move a gate and dispense food portions.
* Display ili9341
* Three push buttons (Enter, UP and DOWN)
* Buzzer

## Project architecture<a name="id2"></a>
![board](/docs/images/project_architecture.jpg)
## App configuration<a name="id3"></a>

In *appConfig.h* you will find C macros to configure the application and peripheral
configurations. This is the place you must to change if you want to implement the app
in a different microcontroller or development board.
## Use example <a name="id4"></a>
[//]: <> (TODO)
