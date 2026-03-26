# ThreadX RTOS library for Cortex-M7
# Equivalent to OS/threadx/Makefile

set(THREADX_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(THREADX_COMMON ${THREADX_ROOT}/common)
set(THREADX_PORT ${THREADX_ROOT}/ports/cortex_m7/gnu)

# Collect ThreadX source files
file(GLOB THREADX_COMMON_SOURCES
    ${THREADX_COMMON}/src/*.c
)
file(GLOB THREADX_PORT_SOURCES
    ${THREADX_PORT}/src/*.c
    ${THREADX_PORT}/src/*.S
)

set(THREADX_SOURCES
    ${THREADX_COMMON_SOURCES}
    ${THREADX_PORT_SOURCES}
)

# ThreadX include directories
set(THREADX_INC_DIRS
    ${THREADX_COMMON}/inc
    ${THREADX_PORT}/inc
    CACHE PATH "ThreadX include directories"
)

# Create static library
add_library(tx_m7 STATIC ${THREADX_SOURCES})

# ThreadX uses -O0 -g (debug build)
target_compile_options(tx_m7 PRIVATE
    ${MCU_FLAGS}
    -O0
    -g
    -ffunction-sections
    -fdata-sections
    -fno-strict-aliasing
    -fno-builtin
    -fshort-enums
    -Wall
)

# Linker options
target_link_options(tx_m7 PRIVATE
    ${MCU_FLAGS}
)

# Include directories
target_include_directories(tx_m7 PUBLIC
    ${THREADX_INC_DIRS}
)

# Disable LTO for this static library
set_target_properties(tx_m7 PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
