################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../thirdParty/freeRTOS/croutine.c \
../thirdParty/freeRTOS/event_groups.c \
../thirdParty/freeRTOS/list.c \
../thirdParty/freeRTOS/queue.c \
../thirdParty/freeRTOS/stream_buffer.c \
../thirdParty/freeRTOS/tasks.c \
../thirdParty/freeRTOS/timers.c 

OBJS += \
./thirdParty/freeRTOS/croutine.o \
./thirdParty/freeRTOS/event_groups.o \
./thirdParty/freeRTOS/list.o \
./thirdParty/freeRTOS/queue.o \
./thirdParty/freeRTOS/stream_buffer.o \
./thirdParty/freeRTOS/tasks.o \
./thirdParty/freeRTOS/timers.o 

C_DEPS += \
./thirdParty/freeRTOS/croutine.d \
./thirdParty/freeRTOS/event_groups.d \
./thirdParty/freeRTOS/list.d \
./thirdParty/freeRTOS/queue.d \
./thirdParty/freeRTOS/stream_buffer.d \
./thirdParty/freeRTOS/tasks.d \
./thirdParty/freeRTOS/timers.d 


# Each subdirectory must supply rules for building sources it contributes
thirdParty/freeRTOS/%.o thirdParty/freeRTOS/%.su: ../thirdParty/freeRTOS/%.c thirdParty/freeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"C:/Users/U62REFW/main_personal/dispenser_ili3241_2_8/workspace/pet_food_dispenser/thirdParty/lib_ili9341" -I../Core/Inc -I"C:/Users/U62REFW/main_personal/dispenser_ili3241_2_8/workspace/pet_food_dispenser/thirdParty/freeRTOS/include" -I"C:/Users/U62REFW/main_personal/dispenser_ili3241_2_8/workspace/pet_food_dispenser/thirdParty/freeRTOS/portable/GCC/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-thirdParty-2f-freeRTOS

clean-thirdParty-2f-freeRTOS:
	-$(RM) ./thirdParty/freeRTOS/croutine.d ./thirdParty/freeRTOS/croutine.o ./thirdParty/freeRTOS/croutine.su ./thirdParty/freeRTOS/event_groups.d ./thirdParty/freeRTOS/event_groups.o ./thirdParty/freeRTOS/event_groups.su ./thirdParty/freeRTOS/list.d ./thirdParty/freeRTOS/list.o ./thirdParty/freeRTOS/list.su ./thirdParty/freeRTOS/queue.d ./thirdParty/freeRTOS/queue.o ./thirdParty/freeRTOS/queue.su ./thirdParty/freeRTOS/stream_buffer.d ./thirdParty/freeRTOS/stream_buffer.o ./thirdParty/freeRTOS/stream_buffer.su ./thirdParty/freeRTOS/tasks.d ./thirdParty/freeRTOS/tasks.o ./thirdParty/freeRTOS/tasks.su ./thirdParty/freeRTOS/timers.d ./thirdParty/freeRTOS/timers.o ./thirdParty/freeRTOS/timers.su

.PHONY: clean-thirdParty-2f-freeRTOS

