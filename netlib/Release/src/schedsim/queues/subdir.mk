################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/schedsim/queues/fifo.c \
../src/schedsim/queues/job.c \
../src/schedsim/queues/measures.c \
../src/schedsim/queues/queue_man.c \
../src/schedsim/queues/rr.c 

OBJS += \
./src/schedsim/queues/fifo.o \
./src/schedsim/queues/job.o \
./src/schedsim/queues/measures.o \
./src/schedsim/queues/queue_man.o \
./src/schedsim/queues/rr.o 

C_DEPS += \
./src/schedsim/queues/fifo.d \
./src/schedsim/queues/job.d \
./src/schedsim/queues/measures.d \
./src/schedsim/queues/queue_man.d \
./src/schedsim/queues/rr.d 


# Each subdirectory must supply rules for building sources it contributes
src/schedsim/queues/%.o: ../src/schedsim/queues/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/iizke/workspace/netlib/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


