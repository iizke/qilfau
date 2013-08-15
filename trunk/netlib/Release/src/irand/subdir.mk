################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/irand/rnddist.c \
../src/irand/rndnum.c 

OBJS += \
./src/irand/rnddist.o \
./src/irand/rndnum.o 

C_DEPS += \
./src/irand/rnddist.d \
./src/irand/rndnum.d 


# Each subdirectory must supply rules for building sources it contributes
src/irand/%.o: ../src/irand/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/iizke/workspace/netlib/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


