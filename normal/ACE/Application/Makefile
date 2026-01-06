# This is a template makefile for building projects with nRF52840
TARGET_NAME:=app
LINKER_NAME:=app
BUILD_DIR:=_build
# Makefile library
MAKE_COMMON:=./../Common

# $(info MAKE_COMMON = $(MAKE_COMMON))

# This is the root project directory and the core directory
ROOT_PROJ:=./

EXT_FLASH_DIR:=$(ROOT_PROJ)/../external_flash
# Core directory for the project
# CORE_DIR:=$(ROOT_PROJ)/../Core/board
# CMSIS_DIR:=$(ROOT_PROJ)/../Core/cmsis/inc
# EXTERNAL_DIR:=$(ROOT_PROJ)/../external
COMMON_DIR:=$(ROOT_PROJ)/../Common
SDK_DIR:=$(ROOT_PROJ)/../SDK/
MID_DIR:=$(ROOT_PROJ)/../Middleware
OS_DIR:=$(ROOT_PROJ)/../OS/threadx

# This is the out file rule
$(BUILD_DIR)/$(TARGET_NAME).out: \
	LINKER_SCRIPT := $(ROOT_PROJ)/$(LINKER_NAME).ld

# This is the Flags for the compiler
CPU=-mcpu=cortex-m7 -mthumb -mabi=aapcs
FPU=-mfloat-abi=hard -mfpu=fpv5-sp-d16
OPT=-O3 -g3
CCFLAGS+=$(CPU) $(FPU) $(OPT)
CCFLAGS+=-ffunction-sections -fdata-sections -fno-strict-aliasing
CCFLAGS+=-fno-builtin -fshort-enums
CCFLAGS+=-Wall -Wno-unused-variable
CCFLAGS+=-DSTM32F746xx

ASMFLAG+=$(CPU) $(FPU) $(OPT)

LDFLAGS+=$(CPU) $(FPU) $(OPT)
LDFLAGS+=-Wl,--gc-sections
LDFLAGS+=--specs=nano.specs
LDFLAGS+=-T$(LINKER_SCRIPT) -mthumb -mabi=aapcs


# SRC_FILES=\
# 	$(ROOT_PROJ)/main.c\
# 	$(ROOT_PROJ)/boot_startup.c\

SRC_EXCLUDE=\
	template_startup.s\

SRC_INCLUDE+=$(wildcard *.c)
SRC_INCLUDE+=$(wildcard *.s)
SRC_INCLUDE+=$(wildcard *.S)
SRC_INCLUDE+=$(wildcard $(MID_DIR)/qspi_flash/w25q128j/*.c)
SRC_INCLUDE+=$(wildcard $(MID_DIR)/qspi_flash/*.c)
SRC_INCLUDE+=$(wildcard $(MID_DIR)/sdram/*.c)
SRC_INCLUDE+=$(wildcard $(ROOT_PROJ)/jzip/*.c)
# SRC_INCLUDE+=$(wildcard $(SDK_DIR)/Device/Src/*.c)
# SRC_INCLUDE+=$(wildcard $(SDK_DIR)/Driver/hal/Src/stm32f7xx_hal.c)
# SRC_INCLUDE+=$(wildcard $(SDK_DIR)/Driver/hal/Src/stm32f7xx_hal_cortex.c)
# SRC_INCLUDE+=$(wildcard $(SDK_DIR)/Driver/hal/Src/stm32f7xx_hal_rcc.c)
# SRC_INCLUDE+=$(wildcard $(SDK_DIR)/Driver/hal/Src/stm32f7xx_hal_rcc_ex.c)
# SRC_INCLUDE+=$(wildcard $(SDK_DIR)/Driver/hal/Src/stm32f7xx_hal_pwr.c)
# SRC_INCLUDE+=$(wildcard $(SDK_DIR)/Driver/hal/Src/stm32f7xx_hal_pwr_ex.c)
# SRC_INCLUDE+=$(wildcard $(SDK_DIR)/Driver/hal/Src/stm32f7xx_hal_qspi.c)
# SRC_INCLUDE+=$(wildcard $(SDK_DIR)/Driver/hal/Src/stm32f7xx_hal_dma.c)
# SRC_INCLUDE+=$(wildcard $(SDK_DIR)/Driver/hal/Src/stm32f7xx_hal_gpio.c)
# SRC_INCLUDE+=$(wildcard $(EXTERNAL_DIR)/segger_rtt/*.c)
# SRC_FILES+=$(wildcard $(OSKERNEL_DIR)/*.s)
SRC_FILES:=$(filter-out $(SRC_EXCLUDE), $(SRC_INCLUDE))

INC_FOLDERS=\
	$(COMMON_DIR)\
	$(SDK_DIR)/Core/Include\
	$(SDK_DIR)/Device/Include\
	$(SDK_DIR)/Driver/hal\
	$(SDK_DIR)/Driver/hal/Inc\
	$(MID_DIR)/qspi_flash/w25q128j\
	$(MID_DIR)/qspi_flash\
	$(MID_DIR)/sdram\
	$(OS_DIR)/common/inc\
	$(OS_DIR)/ports/cortex_m7/gnu/inc\
	$(ROOT_PROJ)/jzip\

LIBS+=$(SDK_DIR)/_build/stm32_lib.a
LIBS+=$(OS_DIR)/_build/tx_m7.a
# LIBS+=-lc -lm

.PHONY:	default help

default: $(TARGET_NAME) 

# Include common makefile. File contains TOOLCHAIN directory and rules for building targets
-include $(MAKE_COMMON)/Makefile.common


# Call function to define targets 
$(foreach target, $(TARGET_NAME), $(call define_target, $(target)))


# Help target
help:
	@echo  help

# # # Flash target
# flash: default
# 	Jlink.exe -if swd -device nRF52840_xxAA -speed 4000 -CommanderScript CommanderScript/build_windows.jlink

# # Erase target
# erase:
# 	Jlink.exe -if swd -device nRF52840_xxAA -speed 4000 -CommanderScript CommanderScript/erase_windows.jlink
flash_bin: default
	STM32_Programmer_CLI.exe -c port=SWD -w $(BUILD_DIR)/$(TARGET_NAME).bin 0x08040000 -v -rst

flash_hex: default
	STM32_Programmer_CLI.exe -c port=SWD -el $(EXT_FLASH_DIR)/W25Q128JVEIQ_STM32F746G-DISCO.stldr -w $(BUILD_DIR)/$(TARGET_NAME).hex 0x90000000 -v -hardRst

erase: default
	STM32_Programmer_CLI.exe -c port=SWD -e all