# SDK library — STM32 HAL and device support
# Equivalent to SDK/Makefile

# Collect SDK source files
file(GLOB SDK_DEVICE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Device/Src/*.c
)
file(GLOB SDK_HAL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Driver/hal/Src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Driver/hal/Src/Legacy/*.c
)

set(SDK_SOURCES
    ${SDK_DEVICE_SOURCES}
    ${SDK_HAL_SOURCES}
)

# SDK include directories
set(SDK_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Device/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Device/Include/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/Driver/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/Driver/hal/Inc
    CACHE PATH "SDK include directories"
)

# Create static library
add_library(stm32_lib STATIC ${SDK_SOURCES})

# SDK uses -O0 -g (debug build)
target_compile_options(stm32_lib PRIVATE
    ${MCU_FLAGS}
    -O0
    -g
    -ffunction-sections
    -fdata-sections
    -fno-strict-aliasing
    -fno-builtin
    -fshort-enums
    -Wall
    -Wno-unused-variable
)

# Linker options for position independent code
target_link_options(stm32_lib PRIVATE
    ${MCU_FLAGS}
)

# Compile definitions
target_compile_definitions(stm32_lib PRIVATE
    STM32F746xx
)

# Include directories
target_include_directories(stm32_lib PUBLIC
    ${SDK_INC_DIRS}
)

# Disable LTO for this static library to avoid linker warnings
set_target_properties(stm32_lib PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
