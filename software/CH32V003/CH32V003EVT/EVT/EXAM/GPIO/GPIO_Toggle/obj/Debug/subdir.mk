################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Project/Snowflake-6/software/CH32V003/CH32V003EVT/EVT/EXAM/SRC/Debug/debug.c 

OBJS += \
./Debug/debug.o 

C_DEPS += \
./Debug/debug.d 


# Each subdirectory must supply rules for building sources it contributes
Debug/debug.o: E:/Project/Snowflake-6/software/CH32V003/CH32V003EVT/EVT/EXAM/SRC/Debug/debug.c
	@	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"E:\Project\Snowflake-6\software\CH32V003\CH32V003EVT\EVT\EXAM\SRC\Debug" -I"E:\Project\Snowflake-6\software\CH32V003\CH32V003EVT\EVT\EXAM\SRC\Core" -I"E:\Project\Snowflake-6\software\CH32V003\CH32V003EVT\EVT\EXAM\GPIO\GPIO_Toggle\User" -I"E:\Project\Snowflake-6\software\CH32V003\CH32V003EVT\EVT\EXAM\SRC\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

