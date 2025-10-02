# This is a template makefile for building projects with nRF52840
TARGET_NAME:=boot
LINKER_NAME:=boot
BUILD_DIR:=_build
# Makefile library
MAKE_COMMON:=./../Common

# $(info MAKE_COMMON = $(MAKE_COMMON))

# This is the root project directory and the core directory
ROOT_PROJ:=./
# Core directory for the project
# CORE_DIR:=$(ROOT_PROJ)/../Core/board
# CMSIS_DIR:=$(ROOT_PROJ)/../Core/cmsis/inc
# EXTERNAL_DIR:=$(ROOT_PROJ)/../external

BOOT_DIR:=$(ROOT_PROJ)/boot_process
COMMON_DIR:=$(ROOT_PROJ)/../Common
SDK_DIR:=$(ROOT_PROJ)/../SDK/

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
SRC_INCLUDE+=$(wildcard $(BOOT_DIR)/*.c)
SRC_INCLUDE+=$(wildcard $(BOOT_DIR)/operation/*.c)
SRC_INCLUDE+=$(wildcard $(SDK_DIR)/Driver/hal/Src/stm32f7xx_hal_rcc.c)
SRC_INCLUDE+=$(wildcard $(SDK_DIR)/Driver/hal/Src/stm32f7xx_hal_rcc_ex.c)
SRC_INCLUDE+=$(wildcard *.s)
# SRC_INCLUDE+=$(wildcard $(EXTERNAL_DIR)/segger_rtt/*.c)
# SRC_FILES+=$(wildcard $(OSKERNEL_DIR)/*.s)
SRC_FILES:=$(filter-out $(SRC_EXCLUDE), $(SRC_INCLUDE))

INC_FOLDERS=\
	$(COMMON_DIR)\
	$(SDK_DIR)/Core/Include\
	$(SDK_DIR)/Device/Include\
	$(SDK_DIR)/Driver/hal/Inc\
	$(BOOT_DIR)\
	$(BOOT_DIR)/operation\

LIBS+=
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
flash: default
	STM32_Programmer_CLI.exe -c port=SWD -w $(BUILD_DIR)/$(TARGET_NAME).bin 0x08000000 -v -rst

erase: default
	STM32_Programmer_CLI.exe -c port=SWD -e all