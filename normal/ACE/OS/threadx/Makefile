
TARGET_NAME=tx_m7
BUILD_DIR=_build
OBJ_DIR=$(BUILD_DIR)/obj
LIB_OUTPUT_PATH = $(BUILD_DIR)/$(TARGET_NAME).a
COMMON=./common/
CORE=./ports/cortex_m7/gnu/

# TOOLCHAIN
CAR=arm-none-eabi-ar
CC=arm-none-eabi-gcc

# CFLAG

CPU=-mcpu=cortex-m7 -mthumb -mabi=aapcs
FPU=-mfloat-abi=hard -mfpu=fpv5-sp-d16
OPT=-O0 -g
CFLAG+= $(CPU) $(FPU) $(FLOAT) $(OPT)

SRC_EXCLUDE=\

SRC_INCLUDE+=$(wildcard $(COMMON)/src/*.c)
SRC_INCLUDE+=$(wildcard $(CORE)/src/*.c)
SRC_INCLUDE+=$(wildcard $(CORE)/src/*.S)

SRC_FILES:=$(filter-out $(SRC_EXCLUDE), $(SRC_INCLUDE))

INC_FOLDERS=\
	$(COMMON)/inc\
	$(CORE)/inc\


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




