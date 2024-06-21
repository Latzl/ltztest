OBJS += $(call GET_OBJS,./gtest/,gtest)

include ../tcli.mk

# gtest
$(OBJ_DIR)/gtest/%.o: ./gtest/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)/gtest
	$(CXX) $(FLAGS) -c $< -o $@
