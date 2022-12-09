################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/lib_ili9341/fonts.c \
../Drivers/lib_ili9341/tft_ili9341.c 

OBJS += \
./Drivers/lib_ili9341/fonts.o \
./Drivers/lib_ili9341/tft_ili9341.o 

C_DEPS += \
./Drivers/lib_ili9341/fonts.d \
./Drivers/lib_ili9341/tft_ili9341.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/lib_ili9341/%.o Drivers/lib_ili9341/%.su: ../Drivers/lib_ili9341/%.c Drivers/lib_ili9341/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I"C:/Users/aaron/main/repos/food-dispenser/workspace/food_dispenser_STM32F401CCU6/freeRTOS/include" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/food_dispenser_STM32F401CCU6/freeRTOS/portable/GCC/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/aaron/main/repos/food-dispenser/workspace/food_dispenser_STM32F401CCU6/Bsp" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/food_dispenser_STM32F401CCU6/App/Inc" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/food_dispenser_STM32F401CCU6/Startup" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/food_dispenser_STM32F401CCU6/Drivers/lib_ili9341" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-lib_ili9341

clean-Drivers-2f-lib_ili9341:
	-$(RM) ./Drivers/lib_ili9341/fonts.d ./Drivers/lib_ili9341/fonts.o ./Drivers/lib_ili9341/fonts.su ./Drivers/lib_ili9341/tft_ili9341.d ./Drivers/lib_ili9341/tft_ili9341.o ./Drivers/lib_ili9341/tft_ili9341.su

.PHONY: clean-Drivers-2f-lib_ili9341

