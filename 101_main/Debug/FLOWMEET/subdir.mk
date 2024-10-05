################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FLOWMEET/fm_init.c \
../FLOWMEET/fm_setup.c \
../FLOWMEET/fm_user.c \
../FLOWMEET/fmx.c \
../FLOWMEET/fmx_lp.c 

OBJS += \
./FLOWMEET/fm_init.o \
./FLOWMEET/fm_setup.o \
./FLOWMEET/fm_user.o \
./FLOWMEET/fmx.o \
./FLOWMEET/fmx_lp.o 

C_DEPS += \
./FLOWMEET/fm_init.d \
./FLOWMEET/fm_setup.d \
./FLOWMEET/fm_user.d \
./FLOWMEET/fmx.d \
./FLOWMEET/fmx_lp.d 


# Each subdirectory must supply rules for building sources it contributes
FLOWMEET/%.o FLOWMEET/%.su FLOWMEET/%.cyclo: ../FLOWMEET/%.c FLOWMEET/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_NON_SECURE=1 -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I"D:/githubs/fmc_320u/101_main/FLOWMEET" -I"D:/githubs/fmc_320u/101_main/libs" -I../AZURE_RTOS/App -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/threadx/common/inc -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -I../Middlewares/ST/threadx/utility/low_power -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-FLOWMEET

clean-FLOWMEET:
	-$(RM) ./FLOWMEET/fm_init.cyclo ./FLOWMEET/fm_init.d ./FLOWMEET/fm_init.o ./FLOWMEET/fm_init.su ./FLOWMEET/fm_setup.cyclo ./FLOWMEET/fm_setup.d ./FLOWMEET/fm_setup.o ./FLOWMEET/fm_setup.su ./FLOWMEET/fm_user.cyclo ./FLOWMEET/fm_user.d ./FLOWMEET/fm_user.o ./FLOWMEET/fm_user.su ./FLOWMEET/fmx.cyclo ./FLOWMEET/fmx.d ./FLOWMEET/fmx.o ./FLOWMEET/fmx.su ./FLOWMEET/fmx_lp.cyclo ./FLOWMEET/fmx_lp.d ./FLOWMEET/fmx_lp.o ./FLOWMEET/fmx_lp.su

.PHONY: clean-FLOWMEET

