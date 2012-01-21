################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Cell.cpp \
../src/Geometry.cpp \
../src/Lattice.cpp \
../src/Material.cpp \
../src/Point.cpp \
../src/Quadrature.cpp \
../src/Surface.cpp \
../src/Timer.cpp \
../src/Track.cpp \
../src/Universe.cpp \
../src/openmoc.cpp 

OBJS += \
./src/Cell.o \
./src/Geometry.o \
./src/Lattice.o \
./src/Material.o \
./src/Point.o \
./src/Quadrature.o \
./src/Surface.o \
./src/Timer.o \
./src/Track.o \
./src/Universe.o \
./src/openmoc.o 

CPP_DEPS += \
./src/Cell.d \
./src/Geometry.d \
./src/Lattice.d \
./src/Material.d \
./src/Point.d \
./src/Quadrature.d \
./src/Surface.d \
./src/Timer.d \
./src/Track.d \
./src/Universe.d \
./src/openmoc.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -Irt -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


