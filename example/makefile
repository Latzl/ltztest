SUB_OBJS += $(call GET_OBJS,./gtest/,gtest)

include ../tcli.mk

$(OBJ_DIR)/gtest/%.o: ./gtest/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)/gtest
	$(CXX) $(FLAGS) -c $< -o $@
