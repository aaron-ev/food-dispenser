################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../thirdParty/freeRTOS/portable/GCC/ARM_CM4F/port.c 

OBJS += \
./thirdParty/freeRTOS/portable/GCC/ARM_CM4F/port.o 

C_DEPS += \
./thirdParty/freeRTOS/portable/GCC/ARM_CM4F/port.d 


# Each subdirectory must supply rules for building sources it contributes
thirdParty/freeRTOS/portable/GCC/ARM_CM4F/%.o thirdParty/freeRTOS/portable/GCC/ARM_CM4F/%.su: ../thirdParty/freeRTOS/portable/GCC/ARM_CM4F/%.c thirdParty/freeRTOS/portable/GCC/ARM_CM4F/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I"C:/Users/U62REFW/main_personal/dispenser_ili3241_2_8/workspace/pet_food_dispenser/thirdParty/freeRTOS/include" -I"C:/Users/U62REFW/main_personal/dispenser_ili3241_2_8/workspace/pet_food_dispenser/thirdParty/freeRTOS/portable/GCC/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-thirdParty-2f-freeRTOS-2f-portable-2f-GCC-2f-ARM_CM4F

clean-thirdParty-2f-freeRTOS-2f-portable-2f-GCC-2f-ARM_CM4F:
	-$(RM) ./thirdParty/freeRTOS/portable/GCC/ARM_CM4F/port.d ./thirdParty/freeRTOS/portable/GCC/ARM_CM4F/port.o ./thirdParty/freeRTOS/portable/GCC/ARM_CM4F/port.su

.PHONY: clean-thirdParty-2f-freeRTOS-2f-portable-2f-GCC-2f-ARM_CM4F

