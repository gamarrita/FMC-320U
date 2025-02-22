################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/fm_debug.c \
../libs/fm_lcd.c \
../libs/fm_lcd_ll.c \
../libs/fm_pcf8553.c 

OBJS += \
./libs/fm_debug.o \
./libs/fm_lcd.o \
./libs/fm_lcd_ll.o \
./libs/fm_pcf8553.o 

C_DEPS += \
./libs/fm_debug.d \
./libs/fm_lcd.d \
./libs/fm_lcd_ll.d \
./libs/fm_pcf8553.d 


# Each subdirectory must supply rules for building sources it contributes
libs/%.o libs/%.su libs/%.cyclo: ../libs/%.c libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I"D:/githubs/FMC-320U/100_main/libs" -I"D:/githubs/FMC-320U/200_pulse_counter/FLOWMEET" -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-libs

clean-libs:
	-$(RM) ./libs/fm_debug.cyclo ./libs/fm_debug.d ./libs/fm_debug.o ./libs/fm_debug.su ./libs/fm_lcd.cyclo ./libs/fm_lcd.d ./libs/fm_lcd.o ./libs/fm_lcd.su ./libs/fm_lcd_ll.cyclo ./libs/fm_lcd_ll.d ./libs/fm_lcd_ll.o ./libs/fm_lcd_ll.su ./libs/fm_pcf8553.cyclo ./libs/fm_pcf8553.d ./libs/fm_pcf8553.o ./libs/fm_pcf8553.su

.PHONY: clean-libs

