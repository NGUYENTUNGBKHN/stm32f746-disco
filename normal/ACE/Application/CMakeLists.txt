# Application — builds app executable
# Equivalent to Application/Makefile

# --------------------------------------------------------------------------
# Sources
# --------------------------------------------------------------------------
file(GLOB APP_C_SRC      "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
file(GLOB APP_ASM_S_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/*.S")
file(GLOB APP_ASM_s_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/*.s")
file(GLOB APP_JZIP_SRC   "${CMAKE_CURRENT_SOURCE_DIR}/jzip/*.c")

# Exclude template_startup.s (lowercase) — same as SRC_EXCLUDE in Makefile
list(FILTER APP_ASM_s_SRC EXCLUDE REGEX "template_startup\\.s$")

add_executable(app
    ${APP_C_SRC}
    ${APP_ASM_S_SRC}
    ${APP_ASM_s_SRC}
    ${APP_JZIP_SRC}
)

# --------------------------------------------------------------------------
# Include directories
# --------------------------------------------------------------------------
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/jzip
    ${CMAKE_SOURCE_DIR}/Common
)

# --------------------------------------------------------------------------
# Compile flags — same as Application/Makefile (-O3 -g3)
# --------------------------------------------------------------------------
target_compile_options(app PRIVATE
    -mcpu=cortex-m7 -mthumb -mabi=aapcs
    -mfloat-abi=hard -mfpu=fpv5-sp-d16
    -O3 -g3
    -ffunction-sections -fdata-sections
    -fno-strict-aliasing -fno-builtin -fshort-enums
    -Wall -Wno-unused-variable
    -DSTM32F746xx
)

# --------------------------------------------------------------------------
# Linker flags — same as Application/Makefile
# --------------------------------------------------------------------------
set(APP_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/app.ld")

target_link_options(app PRIVATE
    -mcpu=cortex-m7 -mthumb -mabi=aapcs
    -mfloat-abi=hard -mfpu=fpv5-sp-d16
    -O3 -g3
    --specs=nano.specs
    -Wl,--gc-sections
    -T${APP_LINKER_SCRIPT}
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/app.map
)

set_target_properties(app PROPERTIES OUTPUT_NAME "app" SUFFIX ".elf")

# --------------------------------------------------------------------------
# Libraries (stm32_lib, tx_m7, middleware — and their transitive includes)
# --------------------------------------------------------------------------
target_link_libraries(app PRIVATE
    stm32_lib
    tx_m7
    middleware
)

# --------------------------------------------------------------------------
# Post-build: generate .bin and .hex
# --------------------------------------------------------------------------
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:app>
            ${CMAKE_CURRENT_BINARY_DIR}/app.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:app>
            ${CMAKE_CURRENT_BINARY_DIR}/app.hex
    COMMENT "Generating app.bin and app.hex"
)

# --------------------------------------------------------------------------
# Flash targets (custom targets, not built by default)
# --------------------------------------------------------------------------
# Flash internal flash (Application partition at 0x08040000)
add_custom_target(flash_bin
    DEPENDS app
    COMMAND STM32_Programmer_CLI.exe -c port=SWD
            -w ${CMAKE_CURRENT_BINARY_DIR}/app.bin 0x08040000 -v -rst
    COMMENT "Flashing app.bin to internal flash at 0x08040000"
)

# Flash external QSPI flash (0x90000000)
add_custom_target(flash_hex
    DEPENDS app
    COMMAND STM32_Programmer_CLI.exe -c port=SWD
            -el ${CMAKE_SOURCE_DIR}/external_flash/W25Q128JVEIQ_STM32F746G-DISCO.stldr
            -w ${CMAKE_CURRENT_BINARY_DIR}/app.hex 0x90000000 -v -hardRst
    COMMENT "Flashing app.hex to external QSPI flash at 0x90000000"
)

# Erase all flash
add_custom_target(erase_flash
    COMMAND STM32_Programmer_CLI.exe -c port=SWD -e all
    COMMENT "Erasing all flash"
)
