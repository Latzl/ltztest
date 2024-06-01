MKDIR = $(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))
CXX = g++
FLAGS = -g -O0 -std=c++11 -Wall
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj
TARGET_DIR = $(BUILD_DIR)/bin
TARGET = $(TARGET_DIR)/libtcli.a

OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(wildcard *.cpp))

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	@mkdir -p $(TARGET_DIR)
	ar -src $@ $^

.PHONY: all clean install
all: $(TARGET)

clean:
	@rm -rvf $(BUILD_DIR)

install: all
	@cp -ivf $(TARGET) /usr/local/lib/
