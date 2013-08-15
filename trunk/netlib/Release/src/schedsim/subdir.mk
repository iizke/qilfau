################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/schedsim/event.c \
../src/schedsim/schedsim.c \
../src/schedsim/sys_aqueue.c \
../src/schedsim/sys_rr_queue.c 

OBJS += \
./src/schedsim/event.o \
./src/schedsim/schedsim.o \
./src/schedsim/sys_aqueue.o \
./src/schedsim/sys_rr_queue.o 

C_DEPS += \
./src/schedsim/event.d \
./src/schedsim/schedsim.d \
./src/schedsim/sys_aqueue.d \
./src/schedsim/sys_rr_queue.d 


# Each subdirectory must supply rules for building sources it contributes
src/schedsim/%.o: ../src/schedsim/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/iizke/workspace/netlib/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


