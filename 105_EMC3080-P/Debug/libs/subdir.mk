################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/githubs/FMC-320U/100_main/libs/fm_backup.c \
D:/githubs/FMC-320U/100_main/libs/fm_debug.c \
D:/githubs/FMC-320U/100_main/libs/fm_factory.c \
D:/githubs/FMC-320U/100_main/libs/fm_flash.c \
D:/githubs/FMC-320U/100_main/libs/fm_fmc.c \
D:/githubs/FMC-320U/100_main/libs/fm_lcd.c \
D:/githubs/FMC-320U/100_main/libs/fm_lcd_ll.c \
D:/githubs/FMC-320U/100_main/libs/fm_logger.c \
D:/githubs/FMC-320U/100_main/libs/fm_pcf8553.c \
D:/githubs/FMC-320U/100_main/libs/fm_rtc.c 

OBJS += \
./libs/fm_backup.o \
./libs/fm_debug.o \
./libs/fm_factory.o \
./libs/fm_flash.o \
./libs/fm_fmc.o \
./libs/fm_lcd.o \
./libs/fm_lcd_ll.o \
./libs/fm_logger.o \
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
./libs/fm_pcf8553.d \
./libs/fm_rtc.d 


# Each subdirectory must supply rules for building sources it contributes
libs/fm_backup.o: D:/githubs/FMC-320U/100_main/libs/fm_backup.c libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
libs/fm_debug.o: D:/githubs/FMC-320U/100_main/libs/fm_debug.c libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
libs/fm_factory.o: D:/githubs/FMC-320U/100_main/libs/fm_factory.c libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
libs/fm_flash.o: D:/githubs/FMC-320U/100_main/libs/fm_flash.c libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
libs/fm_fmc.o: D:/githubs/FMC-320U/100_main/libs/fm_fmc.c libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
libs/fm_lcd.o: D:/githubs/FMC-320U/100_main/libs/fm_lcd.c libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
libs/fm_lcd_ll.o: D:/githubs/FMC-320U/100_main/libs/fm_lcd_ll.c libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
libs/fm_logger.o: D:/githubs/FMC-320U/100_main/libs/fm_logger.c libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
libs/fm_pcf8553.o: D:/githubs/FMC-320U/100_main/libs/fm_pcf8553.c libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
libs/fm_rtc.o: D:/githubs/FMC-320U/100_main/libs/fm_rtc.c libs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-libs

clean-libs:
	-$(RM) ./libs/fm_backup.cyclo ./libs/fm_backup.d ./libs/fm_backup.o ./libs/fm_backup.su ./libs/fm_debug.cyclo ./libs/fm_debug.d ./libs/fm_debug.o ./libs/fm_debug.su ./libs/fm_factory.cyclo ./libs/fm_factory.d ./libs/fm_factory.o ./libs/fm_factory.su ./libs/fm_flash.cyclo ./libs/fm_flash.d ./libs/fm_flash.o ./libs/fm_flash.su ./libs/fm_fmc.cyclo ./libs/fm_fmc.d ./libs/fm_fmc.o ./libs/fm_fmc.su ./libs/fm_lcd.cyclo ./libs/fm_lcd.d ./libs/fm_lcd.o ./libs/fm_lcd.su ./libs/fm_lcd_ll.cyclo ./libs/fm_lcd_ll.d ./libs/fm_lcd_ll.o ./libs/fm_lcd_ll.su ./libs/fm_logger.cyclo ./libs/fm_logger.d ./libs/fm_logger.o ./libs/fm_logger.su ./libs/fm_pcf8553.cyclo ./libs/fm_pcf8553.d ./libs/fm_pcf8553.o ./libs/fm_pcf8553.su ./libs/fm_rtc.cyclo ./libs/fm_rtc.d ./libs/fm_rtc.o ./libs/fm_rtc.su

.PHONY: clean-libs

