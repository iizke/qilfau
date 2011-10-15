################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ranlib/com.c \
../src/ranlib/linpack.c \
../src/ranlib/ranlib.c \
../src/ranlib/ranlib_prb1.c \
../src/ranlib/ranlib_prb2.c \
../src/ranlib/ranlib_prb3.c 

OBJS += \
./src/ranlib/com.o \
./src/ranlib/linpack.o \
./src/ranlib/ranlib.o \
./src/ranlib/ranlib_prb1.o \
./src/ranlib/ranlib_prb2.o \
./src/ranlib/ranlib_prb3.o 

C_DEPS += \
./src/ranlib/com.d \
./src/ranlib/linpack.d \
./src/ranlib/ranlib.d \
./src/ranlib/ranlib_prb1.d \
./src/ranlib/ranlib_prb2.d \
./src/ranlib/ranlib_prb3.d 


# Each subdirectory must supply rules for building sources it contributes
src/ranlib/%.o: ../src/ranlib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/iizke/workspace/netlib/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


