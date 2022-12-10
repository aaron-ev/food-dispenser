################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Bsp/buzzer.c \
../Bsp/console.c \
../Bsp/it.c \
../Bsp/msp.c \
../Bsp/servoMotor.c \
../Bsp/syscalls.c \
../Bsp/sysmem.c \
../Bsp/system_stm32f4xx.c \
../Bsp/timBase.c 

OBJS += \
./Bsp/buzzer.o \
./Bsp/console.o \
./Bsp/it.o \
./Bsp/msp.o \
./Bsp/servoMotor.o \
./Bsp/syscalls.o \
./Bsp/sysmem.o \
./Bsp/system_stm32f4xx.o \
./Bsp/timBase.o 

C_DEPS += \
./Bsp/buzzer.d \
./Bsp/console.d \
./Bsp/it.d \
./Bsp/msp.d \
./Bsp/servoMotor.d \
./Bsp/syscalls.d \
./Bsp/sysmem.d \
./Bsp/system_stm32f4xx.d \
./Bsp/timBase.d 


# Each subdirectory must supply rules for building sources it contributes
Bsp/%.o Bsp/%.su: ../Bsp/%.c Bsp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I"C:/Users/aaron/main/repos/food-dispenser/workspace/foodDispenser/App/Inc" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/foodDispenser/Bsp" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/foodDispenser/Drivers/lib_ili9341" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/foodDispenser/freeRTOS/include" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/foodDispenser/freeRTOS/portable/GCC/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Bsp

clean-Bsp:
	-$(RM) ./Bsp/buzzer.d ./Bsp/buzzer.o ./Bsp/buzzer.su ./Bsp/console.d ./Bsp/console.o ./Bsp/console.su ./Bsp/it.d ./Bsp/it.o ./Bsp/it.su ./Bsp/msp.d ./Bsp/msp.o ./Bsp/msp.su ./Bsp/servoMotor.d ./Bsp/servoMotor.o ./Bsp/servoMotor.su ./Bsp/syscalls.d ./Bsp/syscalls.o ./Bsp/syscalls.su ./Bsp/sysmem.d ./Bsp/sysmem.o ./Bsp/sysmem.su ./Bsp/system_stm32f4xx.d ./Bsp/system_stm32f4xx.o ./Bsp/system_stm32f4xx.su ./Bsp/timBase.d ./Bsp/timBase.o ./Bsp/timBase.su

.PHONY: clean-Bsp

