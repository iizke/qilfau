################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/error.c \
../src/math_util.c \
../src/netlib.c \
../src/network.c \
../src/random.c \
../src/stat_num.c \
../src/vexpr.c 

OBJS += \
./src/error.o \
./src/math_util.o \
./src/netlib.o \
./src/network.o \
./src/random.o \
./src/stat_num.o \
./src/vexpr.o 

C_DEPS += \
./src/error.d \
./src/math_util.d \
./src/netlib.d \
./src/network.d \
./src/random.d \
./src/stat_num.d \
./src/vexpr.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/iizke/workspace/netlib/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


