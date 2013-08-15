################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/tests/subdir.mk
-include src/schedsim/queues/subdir.mk
-include src/schedsim/conf/subdir.mk
-include src/schedsim/subdir.mk
-include src/ranlib/subdir.mk
-include src/polirand/subdir.mk
-include src/poligraph/subdir.mk
-include src/netsimv2/onequeue/subdir.mk
-include src/netsimv2/subdir.mk
-include src/netsim/queues/subdir.mk
-include src/netsim/conf/subdir.mk
-include src/netsim/subdir.mk
-include src/matrix/subdir.mk
-include src/list/subdir.mk
-include src/irand/subdir.mk
-include src/graph/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: pre-build main-build

# Main-build Target
main-build: netlib

# Tool invocations
netlib: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "netlib" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) netlib
	-@echo ' '

pre-build:
	-@echo 'Create config files from Bison/flex'
	-bash /home/iizke/workspace/netlib/src/netsim/conf/makeparser.sh; bash /home/iizke/workspace/netlib/src/schedsim/conf/makeparser.sh
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: main-build pre-build

-include ../makefile.targets
