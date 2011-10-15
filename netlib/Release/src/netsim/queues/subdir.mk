################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/netsim/queues/fifo.c \
../src/netsim/queues/measures.c \
../src/netsim/queues/packet.c \
../src/netsim/queues/queue_man.c 

OBJS += \
./src/netsim/queues/fifo.o \
./src/netsim/queues/measures.o \
./src/netsim/queues/packet.o \
./src/netsim/queues/queue_man.o 

C_DEPS += \
./src/netsim/queues/fifo.d \
./src/netsim/queues/measures.d \
./src/netsim/queues/packet.d \
./src/netsim/queues/queue_man.d 


# Each subdirectory must supply rules for building sources it contributes
src/netsim/queues/%.o: ../src/netsim/queues/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/iizke/workspace/netlib/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


