################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/Src/display.c \
../App/Src/main.c 

OBJS += \
./App/Src/display.o \
./App/Src/main.o 

C_DEPS += \
./App/Src/display.d \
./App/Src/main.d 


# Each subdirectory must supply rules for building sources it contributes
App/Src/%.o App/Src/%.su: ../App/Src/%.c App/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I"C:/Users/aaron/main/repos/food-dispenser/workspace/foodDispenser/App/Inc" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/foodDispenser/Bsp" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/foodDispenser/Drivers/lib_ili9341" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/foodDispenser/freeRTOS/include" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/foodDispenser/freeRTOS/portable/GCC/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-App-2f-Src

clean-App-2f-Src:
	-$(RM) ./App/Src/display.d ./App/Src/display.o ./App/Src/display.su ./App/Src/main.d ./App/Src/main.o ./App/Src/main.su

.PHONY: clean-App-2f-Src

