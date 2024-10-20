TCLI_MKDIR = $(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))
CXX = g++
FLAGS = -g -O0 -std=c++14 -Wall
BUILD_DIR = $(THIS_MKDIR)/build
OBJ_DIR = $(BUILD_DIR)/obj
TARGET_DIR = $(BUILD_DIR)/bin
TARGET = $(TARGET_DIR)/tcli
FLAGS += -fno-access-control

INCS +=

LIBS += -Wl,-Bstatic -ltcli -Wl,-Bdynamic
LIBS += -lpthread -lrt -ldl

# HEADERS += $(wildcard $(TCLI_MKDIR)/third_party/ltz/*/*.hpp)
HEADERS += $(wildcard $(TCLI_MKDIR)/include/tcli/*.hpp)

# call GET_OBJS,src_dir,created_obj_dir
define GET_OBJS
$(patsubst %.cpp, $(OBJ_DIR)/$(2)/%.o,$(notdir $(wildcard $(1)/*.cpp)))
endef

# gtest
OBJS += $(call GET_OBJS,./gtest/,gtest)
$(OBJ_DIR)/gtest/%.o: ./gtest/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)/gtest
	$(CXX) $(INCS) $(FLAGS) -c $< -o $@

# .
HEADERS += $(wildcard ./*.hpp)
OBJS += $(call GET_OBJS,./,.)
$(OBJ_DIR)/%.o: ./%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(INCS) $(FLAGS) -c $< -o $@

# PATH2TCLILIB = $(TCLI_MKDIR)/src/tcli/build/bin/libtcli.a

$(TARGET): $(OBJS)
	@mkdir -p $(TARGET_DIR)
	$(CXX) $(INCS) $(FLAGS) -o $@ $^ $(LIBS)

# $(OBJS): $(PATH2TCLILIB)

.PHONY: all clean test
.DEFAULT_GOAL: all

all : $(TARGET)

clean:
	rm -r $(BUILD_DIR)

test:
	@echo $(THIS_MKDIR)
	@echo $(BUILD_DIR)