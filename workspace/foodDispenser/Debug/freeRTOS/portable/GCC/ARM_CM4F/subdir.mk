################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../freeRTOS/portable/GCC/ARM_CM4F/port.c 

OBJS += \
./freeRTOS/portable/GCC/ARM_CM4F/port.o 

C_DEPS += \
./freeRTOS/portable/GCC/ARM_CM4F/port.d 


# Each subdirectory must supply rules for building sources it contributes
freeRTOS/portable/GCC/ARM_CM4F/%.o freeRTOS/portable/GCC/ARM_CM4F/%.su: ../freeRTOS/portable/GCC/ARM_CM4F/%.c freeRTOS/portable/GCC/ARM_CM4F/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I"C:/Users/aaron/main/repos/food-dispenser/workspace/foodDispenser/App/Inc" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/foodDispenser/Bsp" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/foodDispenser/Drivers/lib_ili9341" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/foodDispenser/freeRTOS/include" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/foodDispenser/freeRTOS/portable/GCC/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-freeRTOS-2f-portable-2f-GCC-2f-ARM_CM4F

clean-freeRTOS-2f-portable-2f-GCC-2f-ARM_CM4F:
	-$(RM) ./freeRTOS/portable/GCC/ARM_CM4F/port.d ./freeRTOS/portable/GCC/ARM_CM4F/port.o ./freeRTOS/portable/GCC/ARM_CM4F/port.su

.PHONY: clean-freeRTOS-2f-portable-2f-GCC-2f-ARM_CM4F

