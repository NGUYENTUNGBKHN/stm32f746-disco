TARGET_NAME=stm32_lib
BUILD_DIR=_build
OBJ_DIR=$(BUILD_DIR)/obj
LIB_OUTPUT_PATH = $(BUILD_DIR)/$(TARGET_NAME).a
COMMON=./common/

DEVICE_DIR:=./Device/
CORE_DIR:=./Core/
DRIVER_DIR:=./Driver/hal/

# TOOLCHAIN
CAR=arm-none-eabi-ar
CC=arm-none-eabi-gcc

# CFLAG

CPU=-mcpu=cortex-m7 -mthumb -mabi=aapcs
FPU=-mfloat-abi=hard -mfpu=fpv5-sp-d16
OPT=-O0 -g
CFLAG+= $(CPU) $(FPU) $(FLOAT) $(OPT)
CFLAG+=-DSTM32F746xx

SRC_EXCLUDE=\

SRC_INCLUDE+=$(wildcard $(DEVICE_DIR)/Src/*.c)
SRC_INCLUDE+=$(wildcard $(DRIVER_DIR)/Src/*.c)
SRC_INCLUDE+=$(wildcard $(DRIVER_DIR)/Src/Legacy/*.c)

SRC_FILES:=$(filter-out $(SRC_EXCLUDE), $(SRC_INCLUDE))

INC_FOLDERS=\
	$(DEVICE_DIR)/Include\
	$(DRIVER_DIR)/Inc\
	$(CORE_DIR)/Include\
	$(DRIVER_DIR)\
	$(DEVICE_DIR)/Include/Legacy\

OBJ_FILES:=$(foreach obj_file, $(SRC_FILES), $(OBJ_DIR)/$(obj_file).o)
# OBJ_FILES = $(patsubst %.c, $(OBJ_DIR)/%.o,$(SRC_FILES))
# OBJ_FILES += $(patsubst %.S, $(OBJ_DIR)/%.S.o,$(SRC_FILES))

# $(info $(OBJ_FILES))

INC+=$(foreach inc, $(INC_FOLDERS), -I$(inc))

.PHONY: all

all: $(LIB_OUTPUT_PATH)

$(OBJ_DIR)/%.c.o: %.c
	@echo "Compiling $(notdir $<)"
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAG) $(INC) -c $< -o $@

$(OBJ_DIR)/%.S.o: %.S
	@echo "Compiling $(notdir $<)"
	@mkdir -p $(shell dirname $@)
	@$(CC) $(CFLAG) $(INC) -c $< -o $@


$(LIB_OUTPUT_PATH): $(OBJ_FILES)
	@echo "Compiling $(notdir $@)"
	@mkdir -p $(shell dirname $@)
	@$(CAR) -r $@ $(OBJ_FILES)  

	
clean: 
	rm -r $(BUILD_DIR)

