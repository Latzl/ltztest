TCLI_MKDIR = $(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))
CXX = g++
FLAGS = -g -O0 -std=c++11 -Wall
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj
TARGET_DIR = $(BUILD_DIR)/bin
TARGET = $(TARGET_DIR)/tcli

LIBS += -Wl,-Bstatic -ltcli -lboost_program_options -lgtest -Wl,-Bdynamic
LIBS += -lpthread -lrt

HEADERS += $(wildcard $(TCLI_MKDIR)/third_party/ltz/*/*.hpp)

define GET_OBJS
$(patsubst %.cpp, $(OBJ_DIR)/$(2)/%.o,$(notdir $(wildcard $(1)/*.cpp)))
endef

OBJS += $(call GET_OBJS,./,.)
$(OBJ_DIR)/%.o: ./%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) -c $< -o $@

PATH2TCLILIB = $(TCLI_MKDIR)/tcli/build/bin/libtcli.a

$(TARGET): $(OBJS) $(PATH2TCLILIB)
	@mkdir -p $(TARGET_DIR)
	$(CXX) $(FLAGS) -o $@ $^ $(LIBS)

.PHONY: clean test
clean:
	rm -r $(BUILD_DIR)

test: 
	@echo $(OBJS)
	@echo $(HEADERS)