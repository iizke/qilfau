################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/poligraph/fault.c \
../src/poligraph/graph.c \
../src/poligraph/main_graph.c \
../src/poligraph/main_rwa.c \
../src/poligraph/matrix.c \
../src/poligraph/print_structures.c \
../src/poligraph/routing.c \
../src/poligraph/rwa.c \
../src/poligraph/topo.c 

OBJS += \
./src/poligraph/fault.o \
./src/poligraph/graph.o \
./src/poligraph/main_graph.o \
./src/poligraph/main_rwa.o \
./src/poligraph/matrix.o \
./src/poligraph/print_structures.o \
./src/poligraph/routing.o \
./src/poligraph/rwa.o \
./src/poligraph/topo.o 

C_DEPS += \
./src/poligraph/fault.d \
./src/poligraph/graph.d \
./src/poligraph/main_graph.d \
./src/poligraph/main_rwa.d \
./src/poligraph/matrix.d \
./src/poligraph/print_structures.d \
./src/poligraph/routing.d \
./src/poligraph/rwa.d \
./src/poligraph/topo.d 


# Each subdirectory must supply rules for building sources it contributes
src/poligraph/%.o: ../src/poligraph/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/iizke/workspace/netlib/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


