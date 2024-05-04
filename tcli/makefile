CXX = g++
FLAGS = -g -O0 -std=c++11 -Wall
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj
TARGET_DIR = $(BUILD_DIR)/bin
TARGET = $(TARGET_DIR)/tcli

LIBS += -Wl,-Bstatic -lboost_program_options -lgtest -Wl,-Bdynamic
# LIBS += -lboost_program_options
LIBS += -lpthread

OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(wildcard *.cpp))

HEADERS += $(wildcard *.hpp)
HEADERS += $(wildcard ../third_party/ltz/*/*.hpp)

$(OBJ_DIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	@mkdir -p $(TARGET_DIR)
	$(CXX) $(FLAGS) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)