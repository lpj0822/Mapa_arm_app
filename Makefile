#************************************************************************
#* Copyright (C) 2015-%%COPYRIGHT_YEAR%% Jungo Connectivity Ltd., All Rights Reserved *
#************************************************************************

ifndef TG
	TG = WISSEN_SE
endif


TOP_DIR := .
CXX = aarch64-linux-gnu-g++
CC = aarch64-linux-gnu-gcc
LD = aarch64-linux-gnu-g++
STRIP = aarch64-linux-gnu-strip
CCACHE_EXT ?= ccache

INC_DIR ?= /usr/inc
BUILD_DIR ?= .build

ifeq ($(TG),WISSEN_SE)
TARGET = record_test_info
endif

BIN_DIR ?= ./bin

C_INC ?= \
	-I. \
	-I$(TOP_DIR) \
	-I$(TOP_DIR)/inc \
	-I$(TOP_DIR)/inc/cnn \
	-I$(TOP_DIR)/inc/alsa \
	-I$(TOP_DIR)/inc/oryx/include \
	-I$(TOP_DIR)/inc/oryx/include/utility \
	-I$(TOP_DIR)/inc/oryx/include/video \
	-I$(TOP_DIR)/inc/oryx/include/ipc \
	-I$(TOP_DIR)/inc/oryx/include/services \
	-I$(TOP_DIR)/inc/wissen

CXX_FLAGS += -Wformat -Werror=format-security -Wall -fPIC -D_REENTRENT \
			 -D_GNU_SOURCE -O3  -fdiagnostics-color=auto -march=armv8-a \
			 -mcpu=cortex-a53+crypto -mlittle-endian -std=c++11 -Werror \
			 -g
C_FLAGS += -Wformat -Werror=format-security -Wall -fPIC -D_REENTRENT \
		   -d_GNU_SOURCE -O3 -fdiagnostics-color=auto -march=armv8-a \
		   -mcpu=cortex-a53+crypto -mlittle-endian -MMD -g

WISSEN_LIB_PATH=./lib
L_FLAGS += -L$(WISSEN_LIB_PATH) -Wl,-rpath=$(WISSEN_LIB_PATH)

ifeq ($(TG),WISSEN_SE)
L_FLAGS += \
	-lamvideo-reader \
	-lamvideo-address \
	-lamvideo \
	-lamutil \
	-lpthread \
	-lSmart \
	-lamba_opt_sc_algos \
	-lasound \
	-lm \
	-ldl \
	-lrt \
	-lamapi \
	-lamipc \
	-lamutil \
	-lamosal
endif

L_FLAGS += -fopenmp

ifeq ($(TG),WISSEN_SE)
OBJECTS += $(addprefix $(BUILD_DIR)/, utils.o LOG_Interface.o SE_Interface.o Audio_Interface.o YUV_Interface.o test_wissen_se.o cnn_verf.o CAN_Interface.o cnn_generic.o)
endif

all: $(BIN_DIR)/$(TARGET)

$(BUILD_DIR):
	$(PSILENT)mkdir -p $@

$(OBJECTS): $(BUILD_DIR)
#	$(PSILENT)rm -rf $(BUILD_DIR)

-include $(OBJECTS:%.o=%.d)

$(BUILD_DIR)/%.o: %.c
	@echo "..Building $< ..."
	$(CXX) $(CXX_FLAGS) $(C_INC) -c -MMD $< -o $@
	@echo "..Done ($<)"

$(BUILD_DIR)/%.o: %.cpp
	@echo "..Building $< ..."
	$(CXX) $(CXX_FLAGS) $(C_INC) -c -MMD $< -o $@
	@echo "..Done ($<)"

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@echo "..Linking $@ ..."
	$(LD) -march=armv8-a -mcpu=cortex-a53+crypto -mlittle-endian -fPIE $(L_FLAGS) -o $@ $(OBJECTS)
	@echo "..Stripping $@ ..."
	$(STRIP) $@

clean: $(BIN_DIR)/$(TARGET)_clean

$(BIN_DIR)/$(TARGET)_clean:
	@echo "..Cleaning $(BIN_DIR)/$(TARGET) ..."
	$(PSILENT)rm -f $(BIN_DIR)/$(TARGET)
	$(PSILENT)rm -rf $(BUILD_DIR)
	@echo "..Done"

help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"

.PHONY: all clean $(BIN_DIR)/$(TARGET)_clean help

