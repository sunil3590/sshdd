################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../test/endian.c \
../test/generate_files.c \
../test/main.c \
../test/profile.c 

OBJS += \
./test/endian.o \
./test/generate_files.o \
./test/main.o \
./test/profile.o 

C_DEPS += \
./test/endian.d \
./test/generate_files.d \
./test/main.d \
./test/profile.d 


# Each subdirectory must supply rules for building sources it contributes
test/%.o: ../test/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"../src" -I"../lib/pqueue" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


