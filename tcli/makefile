MKDIR = $(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))
CXX = g++
FLAGS = -g -O0 -std=c++11 -Wall
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj
TARGET_DIR = $(BUILD_DIR)/bin
TARGET = $(TARGET_DIR)/libtcli.a

HEADERS += $(wildcard $(MKDIR)/../third_party/ltz/*/*.hpp)

# third party
THIRD_PARTY_LIB_DIR = /usr/local/lib

define get_objs_from_static_lib
$(shell ar -t $(1))
endef

# boost program options
OBJ_DIR_THIRD_PARTY_BPO = $(OBJ_DIR)/boost_program_options
SLIB_BPO = $(THIRD_PARTY_LIB_DIR)/libboost_program_options.a
SLIB_BPO := $(shell readlink -f $(SLIB_BPO))
OBJS_BPO = $(call get_objs_from_static_lib,$(SLIB_BPO))
OBJS_BPO := $(patsubst %,$(OBJ_DIR_THIRD_PARTY_BPO)/%,$(OBJS_BPO))
$(OBJS_BPO): $(SLIB_BPO)
	@mkdir -p $(OBJ_DIR_THIRD_PARTY_BPO)
	(cd $(OBJ_DIR_THIRD_PARTY_BPO) && ar -x $(SLIB_BPO))

# gtest
OBJ_DIR_THIRD_PARTY_GTEST = $(OBJ_DIR)/gtest
SLIB_GTEST = $(THIRD_PARTY_LIB_DIR)/libgtest.a
SLIB_GTEST := $(shell readlink -f $(SLIB_GTEST))
OBJS_GTEST = $(call get_objs_from_static_lib,$(SLIB_GTEST))
OBJS_GTEST := $(patsubst %,$(OBJ_DIR_THIRD_PARTY_GTEST)/%,$(OBJS_GTEST))
$(OBJS_GTEST): $(SLIB_GTEST)
	@mkdir -p $(OBJ_DIR_THIRD_PARTY_GTEST)
	(cd $(OBJ_DIR_THIRD_PARTY_GTEST) && ar -x $(SLIB_GTEST))

OBJS_THIRD_PARTY = $(OBJS_BPO) $(OBJS_GTEST)
OBJS += $(OBJS_THIRD_PARTY)


# tcli
OBJS += $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(wildcard *.cpp))

HEADERS += $(wildcard $(MKDIR)/*.hpp)
HEADERS += $(wildcard $(MKDIR)/include/*.hpp)
$(OBJ_DIR)/%.o: ./%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) -c $< -o $@


# target
$(TARGET): $(OBJS) $(OBJS_BPO) 
	@mkdir -p $(TARGET_DIR)
	ar -src $@ $^

.PHONY: all clean install test
.DEFAULT_GOAL = all

test: 
	@echo $(HEADERS)

all: $(TARGET)

clean:
	@rm -rvf $(BUILD_DIR)

install: all
	@cp -ivf $(TARGET) /usr/local/lib/
