# Bootloader — builds boot executable
# Equivalent to Bootloader/Makefile

# --------------------------------------------------------------------------
# Sources
# --------------------------------------------------------------------------
file(GLOB BOOT_C_SRC       "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
# .S (uppercase) is the startup used; .s (lowercase, boot_startup.s) is excluded by Makefile
file(GLOB BOOT_ASM_SRC     "${CMAKE_CURRENT_SOURCE_DIR}/*.S")
file(GLOB BOOT_ASM_s_SRC   "${CMAKE_CURRENT_SOURCE_DIR}/*.s")
file(GLOB BOOT_PROC_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/boot_process/*.c")
file(GLOB BOOT_OP_SRC      "${CMAKE_CURRENT_SOURCE_DIR}/boot_process/operation/*.c")

# Exclude boot_startup.s (lowercase) — same as SRC_EXCLUDE in Makefile
list(FILTER BOOT_ASM_s_SRC EXCLUDE REGEX "boot_startup\\.s$")

add_executable(boot
    ${BOOT_C_SRC}
    ${BOOT_ASM_SRC}
    ${BOOT_ASM_s_SRC}
    ${BOOT_PROC_SRC}
    ${BOOT_OP_SRC}
)

# --------------------------------------------------------------------------
# Include directories
# --------------------------------------------------------------------------
target_include_directories(boot PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/boot_process
    ${CMAKE_CURRENT_SOURCE_DIR}/boot_process/operation
    ${CMAKE_SOURCE_DIR}/Common
)

# --------------------------------------------------------------------------
# Compile flags — same as Bootloader/Makefile (-O3 -g3)
# --------------------------------------------------------------------------
target_compile_options(boot PRIVATE
    -mcpu=cortex-m7 -mthumb -mabi=aapcs
    -mfloat-abi=hard -mfpu=fpv5-sp-d16
    -O3 -g3
    -ffunction-sections -fdata-sections
    -fno-strict-aliasing -fno-builtin -fshort-enums
    -Wall -Wno-unused-variable
    -DSTM32F746xx
)

# --------------------------------------------------------------------------
# Linker flags — same as Bootloader/Makefile
# --------------------------------------------------------------------------
set(BOOT_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/boot.ld")

target_link_options(boot PRIVATE
    -mcpu=cortex-m7 -mthumb -mabi=aapcs
    -mfloat-abi=hard -mfpu=fpv5-sp-d16
    -O3 -g3
    --specs=nano.specs
    -Wl,--gc-sections
    -T${BOOT_LINKER_SCRIPT}
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/boot.map
)

set_target_properties(boot PROPERTIES OUTPUT_NAME "boot" SUFFIX ".elf")

# --------------------------------------------------------------------------
# Libraries
# --------------------------------------------------------------------------
target_link_libraries(boot PRIVATE stm32_lib)

# --------------------------------------------------------------------------
# Post-build: generate .bin and .hex
# --------------------------------------------------------------------------
add_custom_command(TARGET boot POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:boot>
            ${CMAKE_CURRENT_BINARY_DIR}/boot.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:boot>
            ${CMAKE_CURRENT_BINARY_DIR}/boot.hex
    COMMENT "Generating boot.bin and boot.hex"
)

# --------------------------------------------------------------------------
# Flash target (custom target, not built by default)
# --------------------------------------------------------------------------
add_custom_target(flash_boot
    DEPENDS boot
    COMMAND STM32_Programmer_CLI.exe -c port=SWD
            -w ${CMAKE_CURRENT_BINARY_DIR}/boot.bin 0x08000000 -v -rst
    COMMENT "Flashing boot to internal flash at 0x08000000"
)
