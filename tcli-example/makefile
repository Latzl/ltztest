CXX = g++
FLAGS = -g -O0 -std=c++11 -Wall
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj
TARGET_DIR = $(BUILD_DIR)/bin
TARGET = $(TARGET_DIR)/tcli

LIBS += -Wl,-Bstatic -lboost_program_options -lgtest -Wl,-Bdynamic
LIBS += -lpthread

HEADERS += $(wildcard ../third_party/ltz/*/*.hpp)

define GET_OBJS
$(patsubst %.cpp, $(OBJ_DIR)/$(2)/%.o,$(notdir $(wildcard $(1)/*.cpp)))
endef

OBJS += $(call GET_OBJS,./,.)
$(OBJ_DIR)/%.o: ./%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) -c $< -o $@

HEADERS += $(wildcard ./common/*.hpp)
OBJS += $(call GET_OBJS,./common/,common)
$(OBJ_DIR)/common/%.o: ./common/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)/common
	$(CXX) $(FLAGS) -c $< -o $@

HEADERS += $(wildcard ./gtest/*.hpp)
OBJS += $(call GET_OBJS,./gtest/,gtest)
$(OBJ_DIR)/gtest/%.o: ./gtest/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)/gtest
	$(CXX) $(FLAGS) -c $< -o $@


$(TARGET): $(OBJS)
	@mkdir -p $(TARGET_DIR)
	$(CXX) $(FLAGS) -o $@ $^ $(LIBS)

.PHONY: clean test
clean:
	rm -r $(BUILD_DIR)

test: 
	@echo $(OBJS)