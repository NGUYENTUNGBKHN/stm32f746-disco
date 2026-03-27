# Middleware — builds middleware static library
# Equivalent to sources included inline in Application/Makefile

add_library(middleware STATIC)

# --------------------------------------------------------------------------
# Sources
# --------------------------------------------------------------------------
file(GLOB MID_QSPI_SRC     "${CMAKE_CURRENT_SOURCE_DIR}/qspi_flash/*.c")
file(GLOB MID_W25Q128_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/qspi_flash/w25q128j/*.c")
file(GLOB MID_SDRAM_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/sdram/*.c")

target_sources(middleware PRIVATE
    ${MID_QSPI_SRC}
    ${MID_W25Q128_SRC}
    ${MID_SDRAM_SRC}
)

# --------------------------------------------------------------------------
# Include directories (PUBLIC so Application inherits them)
# --------------------------------------------------------------------------
target_include_directories(middleware PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/qspi_flash
    ${CMAKE_CURRENT_SOURCE_DIR}/qspi_flash/w25q128j
    ${CMAKE_CURRENT_SOURCE_DIR}/sdram
    ${CMAKE_SOURCE_DIR}/Common
)

# --------------------------------------------------------------------------
# Compile flags
# --------------------------------------------------------------------------
target_compile_options(middleware PRIVATE
    -mcpu=cortex-m7 -mthumb -mabi=aapcs
    -mfloat-abi=hard -mfpu=fpv5-sp-d16
    -O3 -g3
    -ffunction-sections -fdata-sections
    -DSTM32F746xx
)

# --------------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------------
target_link_libraries(middleware PUBLIC stm32_lib)
