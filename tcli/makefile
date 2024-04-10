CXX = g++
FLAGS = -g -O0 -Wall -std=c++11
TARGET_DIR = ./build
TARGET = $(TARGET_DIR)/tcli

$(TARGET): tcbase.hpp main.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) $(FLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -r $(TARGET)