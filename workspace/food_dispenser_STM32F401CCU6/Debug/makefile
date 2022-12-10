################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include freeRTOS/portable/MemMang/subdir.mk
-include freeRTOS/portable/GCC/ARM_CM4F/subdir.mk
-include freeRTOS/subdir.mk
-include Startup/subdir.mk
-include Drivers/lib_ili9341/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Bsp/subdir.mk
-include App/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := food_dispenser_STM32F401CCU6
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
food_dispenser_STM32F401CCU6.elf \

MAP_FILES += \
food_dispenser_STM32F401CCU6.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
food_dispenser_STM32F401CCU6.list \


# All Target
all: main-build

# Main-build Target
main-build: food_dispenser_STM32F401CCU6.elf secondary-outputs

# Tool invocations
food_dispenser_STM32F401CCU6.elf food_dispenser_STM32F401CCU6.map: $(OBJS) $(USER_OBJS) C:\Users\aaron\main\repos\food-dispenser\workspace\food_dispenser_STM32F401CCU6\STM32F401CCUX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "food_dispenser_STM32F401CCU6.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\aaron\main\repos\food-dispenser\workspace\food_dispenser_STM32F401CCU6\STM32F401CCUX_FLASH.ld" --specs=nosys.specs -Wl,-Map="food_dispenser_STM32F401CCU6.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

food_dispenser_STM32F401CCU6.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "food_dispenser_STM32F401CCU6.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) default.size.stdout food_dispenser_STM32F401CCU6.elf food_dispenser_STM32F401CCU6.list food_dispenser_STM32F401CCU6.map
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
