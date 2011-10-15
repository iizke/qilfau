################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/list/array.c \
../src/list/heap.c \
../src/list/linked_list.c \
../src/list/tree.c 

OBJS += \
./src/list/array.o \
./src/list/heap.o \
./src/list/linked_list.o \
./src/list/tree.o 

C_DEPS += \
./src/list/array.d \
./src/list/heap.d \
./src/list/linked_list.d \
./src/list/tree.d 


# Each subdirectory must supply rules for building sources it contributes
src/list/%.o: ../src/list/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/iizke/workspace/netlib/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


