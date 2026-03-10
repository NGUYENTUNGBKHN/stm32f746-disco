# OS/threadx — builds tx_m7 static library
# Equivalent to OS/threadx/Makefile

add_library(tx_m7 STATIC)

# --------------------------------------------------------------------------
# Sources
# --------------------------------------------------------------------------
file(GLOB TX_COMMON_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/common/src/*.c")
file(GLOB TX_PORT_C_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/ports/cortex_m7/gnu/src/*.c")
file(GLOB TX_PORT_S_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/ports/cortex_m7/gnu/src/*.S")

target_sources(tx_m7 PRIVATE
    ${TX_COMMON_SRC}
    ${TX_PORT_C_SRC}
    ${TX_PORT_S_SRC}
)

# --------------------------------------------------------------------------
# Include directories (PUBLIC so Application can see ThreadX headers)
# --------------------------------------------------------------------------
target_include_directories(tx_m7 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/common/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/cortex_m7/gnu/inc
)

# --------------------------------------------------------------------------
# Compile flags — same as OS/threadx/Makefile (-O0 -g for library)
# --------------------------------------------------------------------------
target_compile_options(tx_m7 PRIVATE
    -mcpu=cortex-m7 -mthumb -mabi=aapcs
    -mfloat-abi=hard -mfpu=fpv5-sp-d16
    -O0 -g
)
