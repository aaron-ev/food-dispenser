################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../freeRTOS/croutine.c \
../freeRTOS/event_groups.c \
../freeRTOS/list.c \
../freeRTOS/queue.c \
../freeRTOS/stream_buffer.c \
../freeRTOS/tasks.c \
../freeRTOS/timers.c 

OBJS += \
./freeRTOS/croutine.o \
./freeRTOS/event_groups.o \
./freeRTOS/list.o \
./freeRTOS/queue.o \
./freeRTOS/stream_buffer.o \
./freeRTOS/tasks.o \
./freeRTOS/timers.o 

C_DEPS += \
./freeRTOS/croutine.d \
./freeRTOS/event_groups.d \
./freeRTOS/list.d \
./freeRTOS/queue.d \
./freeRTOS/stream_buffer.d \
./freeRTOS/tasks.d \
./freeRTOS/timers.d 


# Each subdirectory must supply rules for building sources it contributes
freeRTOS/%.o freeRTOS/%.su: ../freeRTOS/%.c freeRTOS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I"C:/Users/aaron/main/repos/food-dispenser/workspace/food_dispenser_STM32F401CCU6/freeRTOS/include" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/food_dispenser_STM32F401CCU6/freeRTOS/portable/GCC/ARM_CM4F" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/aaron/main/repos/food-dispenser/workspace/food_dispenser_STM32F401CCU6/Bsp" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/food_dispenser_STM32F401CCU6/App/Inc" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/food_dispenser_STM32F401CCU6/Startup" -I"C:/Users/aaron/main/repos/food-dispenser/workspace/food_dispenser_STM32F401CCU6/Drivers/lib_ili9341" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-freeRTOS

clean-freeRTOS:
	-$(RM) ./freeRTOS/croutine.d ./freeRTOS/croutine.o ./freeRTOS/croutine.su ./freeRTOS/event_groups.d ./freeRTOS/event_groups.o ./freeRTOS/event_groups.su ./freeRTOS/list.d ./freeRTOS/list.o ./freeRTOS/list.su ./freeRTOS/queue.d ./freeRTOS/queue.o ./freeRTOS/queue.su ./freeRTOS/stream_buffer.d ./freeRTOS/stream_buffer.o ./freeRTOS/stream_buffer.su ./freeRTOS/tasks.d ./freeRTOS/tasks.o ./freeRTOS/tasks.su ./freeRTOS/timers.d ./freeRTOS/timers.o ./freeRTOS/timers.su

.PHONY: clean-freeRTOS

