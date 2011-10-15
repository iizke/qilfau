################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/netsim/csma.c \
../src/netsim/event.c \
../src/netsim/netqthread.c \
../src/netsim/netqueue.c \
../src/netsim/netsim.c \
../src/netsim/sys_aqueue.c 

OBJS += \
./src/netsim/csma.o \
./src/netsim/event.o \
./src/netsim/netqthread.o \
./src/netsim/netqueue.o \
./src/netsim/netsim.o \
./src/netsim/sys_aqueue.o 

C_DEPS += \
./src/netsim/csma.d \
./src/netsim/event.d \
./src/netsim/netqthread.d \
./src/netsim/netqueue.d \
./src/netsim/netsim.d \
./src/netsim/sys_aqueue.d 


# Each subdirectory must supply rules for building sources it contributes
src/netsim/%.o: ../src/netsim/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/iizke/workspace/netlib/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


