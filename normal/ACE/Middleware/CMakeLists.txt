# Middleware library — QSPI flash and SDRAM drivers

set(MID_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Collect Middleware source files
set(MIDDLEWARE_SOURCES
    ${MID_DIR}/qspi_flash/qspi_flash.c
    ${MID_DIR}/qspi_flash/w25q128j/w25q128j.c
    ${MID_DIR}/sdram/sdram.c
)

# Middleware include directories
set(MIDDLEWARE_INC_DIRS
    ${MID_DIR}/qspi_flash
    ${MID_DIR}/qspi_flash/w25q128j
    ${MID_DIR}/sdram
    CACHE PATH "Middleware include directories"
)

# Create static library
add_library(middleware STATIC ${MIDDLEWARE_SOURCES})

# Middleware uses -O3 -g3 (optimized build, same as Application)
target_compile_options(middleware PRIVATE
    ${MCU_FLAGS}
    -O3
    -g3
    -ffunction-sections
    -fdata-sections
    -fno-strict-aliasing
    -fno-builtin
    -fshort-enums
    -Wall
    -Wno-unused-variable
)

# Linker options
target_link_options(middleware PRIVATE
    ${MCU_FLAGS}
)

# Compile definitions
target_compile_definitions(middleware PRIVATE
    STM32F746xx
)

# Include directories
target_include_directories(middleware PUBLIC
    ${MIDDLEWARE_INC_DIRS}
    ${SDK_INC_DIRS}
)

# Link against SDK library
target_link_libraries(middleware PUBLIC
    stm32_lib
)

# Disable LTO for this static library
set_target_properties(middleware PROPERTIES INTERPROCEDURAL_OPTIMIZATION FALSE)
