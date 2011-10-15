################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/matrix/matrix.c 

OBJS += \
./src/matrix/matrix.o 

C_DEPS += \
./src/matrix/matrix.d 


# Each subdirectory must supply rules for building sources it contributes
src/matrix/%.o: ../src/matrix/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/iizke/workspace/netlib/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


