################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/netsim/conf/channellexer.c \
../src/netsim/conf/channelparser.c \
../src/netsim/conf/config.c \
../src/netsim/conf/lexer.c \
../src/netsim/conf/markov_lexer.c \
../src/netsim/conf/markov_parser.c \
../src/netsim/conf/parser.c 

OBJS += \
./src/netsim/conf/channellexer.o \
./src/netsim/conf/channelparser.o \
./src/netsim/conf/config.o \
./src/netsim/conf/lexer.o \
./src/netsim/conf/markov_lexer.o \
./src/netsim/conf/markov_parser.o \
./src/netsim/conf/parser.o 

C_DEPS += \
./src/netsim/conf/channellexer.d \
./src/netsim/conf/channelparser.d \
./src/netsim/conf/config.d \
./src/netsim/conf/lexer.d \
./src/netsim/conf/markov_lexer.d \
./src/netsim/conf/markov_parser.d \
./src/netsim/conf/parser.d 


# Each subdirectory must supply rules for building sources it contributes
src/netsim/conf/%.o: ../src/netsim/conf/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/iizke/workspace/netlib/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


