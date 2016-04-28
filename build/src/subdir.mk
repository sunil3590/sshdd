################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/allocation_strategy.c \
../src/allocation_strategy_helper.c \
../src/file_md.c \
../src/sshdd.c \
../src/sshdd_helper.c 

OBJS += \
./src/allocation_strategy.o \
./src/allocation_strategy_helper.o \
./src/file_md.o \
./src/sshdd.o \
./src/sshdd_helper.o 

C_DEPS += \
./src/allocation_strategy.d \
./src/allocation_strategy_helper.d \
./src/file_md.d \
./src/sshdd.d \
./src/sshdd_helper.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"../src" -I"../lib/pqueue" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


