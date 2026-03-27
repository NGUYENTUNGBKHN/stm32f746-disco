# SDK — builds stm32_lib static library
# Equivalent to SDK/Makefile

add_library(stm32_lib STATIC)

# --------------------------------------------------------------------------
# Sources
# --------------------------------------------------------------------------
file(GLOB SDK_DEVICE_SRC   "${CMAKE_CURRENT_SOURCE_DIR}/Device/Src/*.c")
file(GLOB SDK_HAL_SRC      "${CMAKE_CURRENT_SOURCE_DIR}/Driver/hal/Src/*.c")
file(GLOB SDK_HAL_LEG_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/Driver/hal/Src/Legacy/*.c")

target_sources(stm32_lib PRIVATE
    ${SDK_DEVICE_SRC}
    ${SDK_HAL_SRC}
    ${SDK_HAL_LEG_SRC}
)

# --------------------------------------------------------------------------
# Include directories (PUBLIC so dependents inherit them)
# --------------------------------------------------------------------------
target_include_directories(stm32_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Device/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Device/Include/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/Driver/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/Driver/hal/Inc
)

# --------------------------------------------------------------------------
# Compile flags — same as SDK/Makefile (-O0 -g for library)
# --------------------------------------------------------------------------
target_compile_options(stm32_lib PRIVATE
    -mcpu=cortex-m7 -mthumb -mabi=aapcs
    -mfloat-abi=hard -mfpu=fpv5-sp-d16
    -O0 -g
    -DSTM32F746xx
)
