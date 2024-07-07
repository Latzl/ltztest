HEADERS += $(wildcard $(TCLI_MKDIR)/test/ltz/*.hpp)
OBJS += $(call GET_OBJS,./ltz/string/,ltz/string)

include ../tcli.mk

# ltz
# string
$(OBJ_DIR)/ltz/string/%.o: ./ltz/string/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)/ltz/string
	$(CXX) $(FLAGS) -c $< -o $@