################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/schedsim/conf/config.c \
../src/schedsim/conf/lexer.c \
../src/schedsim/conf/parser.c 

OBJS += \
./src/schedsim/conf/config.o \
./src/schedsim/conf/lexer.o \
./src/schedsim/conf/parser.o 

C_DEPS += \
./src/schedsim/conf/config.d \
./src/schedsim/conf/lexer.d \
./src/schedsim/conf/parser.d 


# Each subdirectory must supply rules for building sources it contributes
src/schedsim/conf/%.o: ../src/schedsim/conf/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/iizke/workspace/netlib/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


