################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/debug/fm_debug.c 

OBJS += \
./libs/debug/fm_debug.o 

C_DEPS += \
./libs/debug/fm_debug.d 


# Each subdirectory must supply rules for building sources it contributes
libs/debug/%.o libs/debug/%.su libs/debug/%.cyclo: ../libs/debug/%.c libs/debug/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_NON_SECURE=1 -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I"D:/githubs/FMC-320U/100_main/libs/debug" -I"D:/githubs/FMC-320U/100_main/FLOWMEET" -I"D:/githubs/FMC-320U/100_main/libs" -I../AZURE_RTOS/App -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/threadx/common/inc -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -I../Middlewares/ST/threadx/utility/low_power -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-libs-2f-debug

clean-libs-2f-debug:
	-$(RM) ./libs/debug/fm_debug.cyclo ./libs/debug/fm_debug.d ./libs/debug/fm_debug.o ./libs/debug/fm_debug.su

.PHONY: clean-libs-2f-debug

