################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../thirdParty/freeRTOS/portable/MemMang/heap_4.c 

OBJS += \
./thirdParty/freeRTOS/portable/MemMang/heap_4.o 

C_DEPS += \
./thirdParty/freeRTOS/portable/MemMang/heap_4.d 


# Each subdirectory must supply rules for building sources it contributes
thirdParty/freeRTOS/portable/MemMang/%.o thirdParty/freeRTOS/portable/MemMang/%.su: ../thirdParty/freeRTOS/portable/MemMang/%.c thirdParty/freeRTOS/portable/MemMang/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"C:/Users/U62REFW/main_personal/dispenser_ili3241_2_8/workspace/pet_food_dispenser/thirdParty/lib_ili9341" -I../Core/Inc -I"C:/Users/U62REFW/main_personal/dispenser_ili3241_2_8/workspace/pet_food_dispenser/thirdParty/freeRTOS/include" -I"C:/Users/U62REFW/main_personal/dispenser_ili3241_2_8/workspace/pet_food_dispenser/thirdParty/freeRTOS/portable/GCC/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-thirdParty-2f-freeRTOS-2f-portable-2f-MemMang

clean-thirdParty-2f-freeRTOS-2f-portable-2f-MemMang:
	-$(RM) ./thirdParty/freeRTOS/portable/MemMang/heap_4.d ./thirdParty/freeRTOS/portable/MemMang/heap_4.o ./thirdParty/freeRTOS/portable/MemMang/heap_4.su

.PHONY: clean-thirdParty-2f-freeRTOS-2f-portable-2f-MemMang

