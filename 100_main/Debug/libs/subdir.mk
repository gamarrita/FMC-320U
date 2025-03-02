################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/fm_backup.c \
../libs/fm_debug.c \
../libs/fm_factory.c \
../libs/fm_flash.c \
../libs/fm_fmc.c \
../libs/fm_lcd.c \
../libs/fm_lcd_ll.c \
../libs/fm_logger.c \
../libs/fm_mxc.c \
../libs/fm_pcf8553.c \
../libs/fm_rtc.c 

OBJS += \
./libs/fm_backup.o \
./libs/fm_debug.o \
./libs/fm_factory.o \
./libs/fm_flash.o \
./libs/fm_fmc.o \
./libs/fm_lcd.o \
./libs/fm_lcd_ll.o \
./libs/fm_logger.o \
./libs/fm_mxc.o \
./libs/fm_pcf8553.o \
./libs/fm_rtc.o 

C_DEPS += \
./libs/fm_backup.d \
./libs/fm_debug.d \
./libs/fm_factory.d \
./libs/fm_flash.d \
./libs/fm_fmc.d \
./libs/fm_lcd.d \
./libs/fm_lcd_ll.d \
./libs/fm_logger.d \
./libs/fm_mxc.d \
./libs/fm_pcf8553.d \
./libs/fm_rtc.d 


# Each subdirectory must supply rules for building sources it contributes
libs/%.o libs/%.su libs/%.cyclo: ../libs/%.c libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_NON_SECURE=1 -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I"D:/githubs/FMC-320U/100_main/FLOWMEET" -I"D:/githubs/FMC-320U/100_main/libs" -I../AZURE_RTOS/App -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/threadx/common/inc -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -I../Middlewares/ST/threadx/utility/low_power -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-libs

clean-libs:
	-$(RM) ./libs/fm_backup.cyclo ./libs/fm_backup.d ./libs/fm_backup.o ./libs/fm_backup.su ./libs/fm_debug.cyclo ./libs/fm_debug.d ./libs/fm_debug.o ./libs/fm_debug.su ./libs/fm_factory.cyclo ./libs/fm_factory.d ./libs/fm_factory.o ./libs/fm_factory.su ./libs/fm_flash.cyclo ./libs/fm_flash.d ./libs/fm_flash.o ./libs/fm_flash.su ./libs/fm_fmc.cyclo ./libs/fm_fmc.d ./libs/fm_fmc.o ./libs/fm_fmc.su ./libs/fm_lcd.cyclo ./libs/fm_lcd.d ./libs/fm_lcd.o ./libs/fm_lcd.su ./libs/fm_lcd_ll.cyclo ./libs/fm_lcd_ll.d ./libs/fm_lcd_ll.o ./libs/fm_lcd_ll.su ./libs/fm_logger.cyclo ./libs/fm_logger.d ./libs/fm_logger.o ./libs/fm_logger.su ./libs/fm_mxc.cyclo ./libs/fm_mxc.d ./libs/fm_mxc.o ./libs/fm_mxc.su ./libs/fm_pcf8553.cyclo ./libs/fm_pcf8553.d ./libs/fm_pcf8553.o ./libs/fm_pcf8553.su ./libs/fm_rtc.cyclo ./libs/fm_rtc.d ./libs/fm_rtc.o ./libs/fm_rtc.su

.PHONY: clean-libs

