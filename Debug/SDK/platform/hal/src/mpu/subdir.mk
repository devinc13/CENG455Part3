################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SDK/platform/hal/src/mpu/fsl_mpu_hal.c 

OBJS += \
./SDK/platform/hal/src/mpu/fsl_mpu_hal.o 

C_DEPS += \
./SDK/platform/hal/src/mpu/fsl_mpu_hal.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/platform/hal/src/mpu/%.o: ../SDK/platform/hal/src/mpu/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"CPU_MK64FN1M0VLL12" -D"FSL_RTOS_MQX" -D"PEX_MQX_KSDK" -I"C:/Users/devinc/ceng455part3/SDK/platform/hal/inc" -I"C:/Users/devinc/ceng455part3/SDK/platform/hal/src/sim/MK64F12" -I"C:/Users/devinc/ceng455part3/SDK/platform/system/src/clock/MK64F12" -I"C:/Users/devinc/ceng455part3/SDK/platform/system/inc" -I"C:/Users/devinc/ceng455part3/SDK/platform/osa/inc" -I"C:/Users/devinc/ceng455part3/SDK/platform/CMSIS/Include" -I"C:/Users/devinc/ceng455part3/SDK/platform/devices" -I"C:/Users/devinc/ceng455part3/SDK/platform/devices/MK64F12/include" -I"C:/Users/devinc/ceng455part3/SDK/platform/utilities/src" -I"C:/Users/devinc/ceng455part3/SDK/platform/utilities/inc" -I"C:/Users/devinc/ceng455part3/SDK/platform/devices/MK64F12/startup" -I"C:/Users/devinc/ceng455part3/Generated_Code/SDK/platform/devices/MK64F12/startup" -I"C:/Users/devinc/ceng455part3/Sources" -I"C:/Users/devinc/ceng455part3/Generated_Code" -I"C:/Users/devinc/ceng455part3/SDK/platform/drivers/inc" -I"C:/Users/devinc/ceng455part3/SDK/rtos/mqx/mqx/source/psp/cortex_m" -I"C:/Users/devinc/ceng455part3/SDK/rtos/mqx/mqx/source/psp/cortex_m/cpu" -I"C:/Users/devinc/ceng455part3/SDK/rtos/mqx/config/common" -I"C:/Users/devinc/ceng455part3/SDK/rtos/mqx/mqx/source/include" -I"C:/Users/devinc/ceng455part3/SDK/rtos/mqx/mqx/source/bsp" -I"C:/Users/devinc/ceng455part3/SDK/rtos/mqx/mqx/source/psp/cortex_m/compiler/gcc_arm" -I"C:/Users/devinc/ceng455part3/SDK/rtos/mqx/mqx/source/nio" -I"C:/Users/devinc/ceng455part3/SDK/rtos/mqx/mqx/source/nio/src" -I"C:/Users/devinc/ceng455part3/SDK/rtos/mqx/mqx/source/nio/fs" -I"C:/Users/devinc/ceng455part3/SDK/rtos/mqx/mqx/source/nio/drivers/nio_dummy" -I"C:/Users/devinc/ceng455part3/SDK/rtos/mqx/mqx/source/nio/drivers/nio_serial" -I"C:/Users/devinc/ceng455part3/SDK/rtos/mqx/mqx/source/nio/drivers/nio_tty" -I"C:/Users/devinc/ceng455part3/SDK/rtos/mqx/mqx_stdlib/source/include" -I"C:/Users/devinc/ceng455part3/SDK/rtos/mqx/mqx_stdlib/source/stdio" -I"C:/Users/devinc/ceng455part3/SDK/platform/hal/src/uart" -I"C:/Users/devinc/ceng455part3/SDK/platform/drivers/src/uart" -I"C:/Users/devinc/ceng455part3/Generated_Code/SDK/rtos/mqx/config/board" -I"C:/Users/devinc/ceng455part3/Generated_Code/SDK/rtos/mqx/config/mcu" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


