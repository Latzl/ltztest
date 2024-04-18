CXX = g++
FLAGS = -g -O0 -Wall -std=c++11
TARGET_DIR = ./build
TARGET = $(TARGET_DIR)/tcli

LIBS += -Wl,-Bstatic -lboost_program_options -Wl,-Bdynamic
# LIBS += -lboost_program_options

$(TARGET): tcbase.hpp main.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) $(FLAGS) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	rm -r $(TARGET)