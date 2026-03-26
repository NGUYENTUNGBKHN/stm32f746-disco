# Application executable
# Equivalent to Application/Makefile

set(APP_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(MID_DIR ${CMAKE_SOURCE_DIR}/Middleware)
set(OS_DIR ${CMAKE_SOURCE_DIR}/OS/threadx)

# Collect Application source files
file(GLOB APP_SOURCES
    ${APP_DIR}/*.c
    ${APP_DIR}/*.S
    ${APP_DIR}/jzip/*.c
)

# Filter out template_startup.s (assembler version), using startup.c instead
list(FILTER APP_SOURCES EXCLUDE REGEX "template_startup\\.s$")

# Add Middleware sources (built as part of app in Makefile)
list(APPEND APP_SOURCES
    ${MID_DIR}/qspi_flash/qspi_flash.c
    ${MID_DIR}/qspi_flash/w25q128j/w25q128j.c
    ${MID_DIR}/sdram/sdram.c
)

# Application include directories
set(APPLICATION_INC_DIRS
    ${CMAKE_SOURCE_DIR}/Common
    ${CMAKE_SOURCE_DIR}/SDK/Core/Include
    ${CMAKE_SOURCE_DIR}/SDK/Device/Include
    ${CMAKE_SOURCE_DIR}/SDK/Driver/hal
    ${CMAKE_SOURCE_DIR}/SDK/Driver/hal/Inc
    ${MID_DIR}/qspi_flash/w25q128j
    ${MID_DIR}/qspi_flash
    ${MID_DIR}/sdram
    ${OS_DIR}/common/inc
    ${OS_DIR}/ports/cortex_m7/gnu/inc
    ${APP_DIR}/jzip
    CACHE PATH "Application include directories"
)

# Linker script
set(APP_LINKER_SCRIPT ${APP_DIR}/app.ld)

# External flash directory (for flashing)
set(EXT_FLASH_DIR ${CMAKE_SOURCE_DIR}/external_flash)

# Create executable
add_executable(app ${APP_SOURCES})

# Application uses -O3 -g3 (optimized build)
target_compile_options(app PRIVATE
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

# Compile definitions
target_compile_definitions(app PRIVATE
    STM32F746xx
)

# Include directories
target_include_directories(app PRIVATE
    ${APPLICATION_INC_DIRS}
)

# Linker options
target_link_options(app PRIVATE
    ${MCU_FLAGS}
    -T${APP_LINKER_SCRIPT}
    -Wl,--gc-sections
    --specs=nano.specs
    -mthumb
    -mabi=aapcs
)

# Link against SDK and ThreadX libraries
target_link_libraries(app PRIVATE
    stm32_lib
    tx_m7
)

# Set output name
set_target_properties(app PROPERTIES
    OUTPUT_NAME "app"
    SUFFIX ".elf"
)

# Post-build: generate .bin and .hex files
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:app> ${CMAKE_CURRENT_BINARY_DIR}/app.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:app> ${CMAKE_CURRENT_BINARY_DIR}/app.hex
    COMMENT "Generating app.bin and app.hex"
)

# Custom target to flash application to internal flash (0x08040000)
add_custom_target(flash_bin
    COMMAND STM32_Programmer_CLI.exe -c port=SWD -w ${CMAKE_CURRENT_BINARY_DIR}/app.bin 0x08040000 -v -rst
    DEPENDS app
    COMMENT "Flashing app to internal flash 0x08040000"
)

# Custom target to flash application to external QSPI flash (0x90000000)
add_custom_target(flash_hex
    COMMAND STM32_Programmer_CLI.exe -c port=SWD -el ${EXT_FLASH_DIR}/W25Q128JVEIQ_STM32F746G-DISCO.stldr -w ${CMAKE_CURRENT_BINARY_DIR}/app.hex 0x90000000 -v -hardRst
    DEPENDS app
    COMMENT "Flashing app to external QSPI flash 0x90000000"
)
