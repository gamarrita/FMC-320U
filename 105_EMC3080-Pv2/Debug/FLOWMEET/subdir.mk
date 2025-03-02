################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FLOWMEET/fm_debug.c \
../FLOWMEET/fm_main.c \
../FLOWMEET/fm_mxc.c 

OBJS += \
./FLOWMEET/fm_debug.o \
./FLOWMEET/fm_main.o \
./FLOWMEET/fm_mxc.o 

C_DEPS += \
./FLOWMEET/fm_debug.d \
./FLOWMEET/fm_main.d \
./FLOWMEET/fm_mxc.d 


# Each subdirectory must supply rules for building sources it contributes
FLOWMEET/%.o FLOWMEET/%.su FLOWMEET/%.cyclo: ../FLOWMEET/%.c FLOWMEET/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I"D:/githubs/FMC-320U/105_EMC3080-Pv2/FLOWMEET" -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-FLOWMEET

clean-FLOWMEET:
	-$(RM) ./FLOWMEET/fm_debug.cyclo ./FLOWMEET/fm_debug.d ./FLOWMEET/fm_debug.o ./FLOWMEET/fm_debug.su ./FLOWMEET/fm_main.cyclo ./FLOWMEET/fm_main.d ./FLOWMEET/fm_main.o ./FLOWMEET/fm_main.su ./FLOWMEET/fm_mxc.cyclo ./FLOWMEET/fm_mxc.d ./FLOWMEET/fm_mxc.o ./FLOWMEET/fm_mxc.su

.PHONY: clean-FLOWMEET

