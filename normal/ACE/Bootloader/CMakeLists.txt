# Bootloader executable
# Equivalent to Bootloader/Makefile

set(BOOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BOOT_PROCESS_DIR ${BOOT_DIR}/boot_process)

# Collect Bootloader source files (excluding boot_startup.s, using startup.c instead)
file(GLOB BOOT_SOURCES
    ${BOOT_DIR}/*.c
    ${BOOT_PROCESS_DIR}/*.c
    ${BOOT_PROCESS_DIR}/operation/*.c
)

# Filter out boot_startup.s (assembler version), use startup.c instead
list(FILTER BOOT_SOURCES EXCLUDE REGEX "boot_startup\\.s$")

# Bootloader include directories
set(BOOTLOADER_INC_DIRS
    ${CMAKE_SOURCE_DIR}/Common
    ${CMAKE_SOURCE_DIR}/SDK/Core/Include
    ${CMAKE_SOURCE_DIR}/SDK/Device/Include
    ${CMAKE_SOURCE_DIR}/SDK/Driver/hal
    ${CMAKE_SOURCE_DIR}/SDK/Driver/hal/Inc
    ${BOOT_DIR}
    ${BOOT_PROCESS_DIR}
    ${BOOT_PROCESS_DIR}/operation
    CACHE PATH "Bootloader include directories"
)

# Linker script
set(BOOT_LINKER_SCRIPT ${BOOT_DIR}/boot.ld)

# Create executable
add_executable(boot ${BOOT_SOURCES})

# Bootloader uses -O3 -g3 (optimized build)
target_compile_options(boot PRIVATE
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
target_compile_definitions(boot PRIVATE
    STM32F746xx
)

# Include directories
target_include_directories(boot PRIVATE
    ${BOOTLOADER_INC_DIRS}
)

# Linker options
target_link_options(boot PRIVATE
    ${MCU_FLAGS}
    -T${BOOT_LINKER_SCRIPT}
    -Wl,--gc-sections
    --specs=nano.specs
    -mthumb
    -mabi=aapcs
)

# Link against SDK library
target_link_libraries(boot PRIVATE
    stm32_lib
)

# Set output name
set_target_properties(boot PROPERTIES
    OUTPUT_NAME "boot"
    SUFFIX ".elf"
)

# Post-build: generate .bin and .hex files
add_custom_command(TARGET boot POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:boot> ${CMAKE_CURRENT_BINARY_DIR}/boot.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:boot> ${CMAKE_CURRENT_BINARY_DIR}/boot.hex
    COMMENT "Generating boot.bin and boot.hex"
)

# Custom target to flash the bootloader
cmake_path(GET CMAKE_CURRENT_BINARY_DIR FILENAME BUILD_DIR_NAME)
add_custom_target(flash_boot
    COMMAND STM32_Programmer_CLI.exe -c port=SWD -w ${CMAKE_CURRENT_BINARY_DIR}/boot.bin 0x08000000 -v -rst
    DEPENDS boot
    COMMENT "Flashing bootloader to 0x08000000"
)

# Custom target to erase the device
add_custom_target(erase
    COMMAND STM32_Programmer_CLI.exe -c port=SWD -e all
    COMMENT "Erasing all flash memory"
)
