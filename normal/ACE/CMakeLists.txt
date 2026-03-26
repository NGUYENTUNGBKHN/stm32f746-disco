cmake_minimum_required(VERSION 3.20)

# Must be set BEFORE project() so toolchain is picked up
# Usage: cmake -B _cmake_build -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake

project(ACE C ASM)

# --------------------------------------------------------------------------
# Global MCU flags (shared by all targets)
# --------------------------------------------------------------------------
# Use CMake list (semicolon-separated) for proper argument handling
set(MCU_FLAGS
    -mcpu=cortex-m7
    -mthumb
    -mabi=aapcs
    -mfloat-abi=hard
    -mfpu=fpv5-sp-d16
    CACHE STRING "MCU compiler flags"
)

# Make CMAKE_OBJCOPY available to subdirectories
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy HINTS $ENV{TOOLCHAIN_TUNG})

# --------------------------------------------------------------------------
# Subdirectories — order matters (dependencies first)
# --------------------------------------------------------------------------
add_subdirectory(SDK)
add_subdirectory(OS/threadx)
add_subdirectory(Middleware)
add_subdirectory(Bootloader)
add_subdirectory(Application)
