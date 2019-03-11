SRC_DIR := $(PWD)/src/
OBJ_DIR := $(PWD)/obj/
BIN_DIR := $(PWD)/bin/
DEP_DIR := $(PWD)/dep/
LIB_DIR := $(PWD)/lib/lib
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(LIB_DIR)
INC_DIR := $(SRC_DIR) -I./3dp/plog/include -I./lib/include -I./3dp/pistache/third-party/rapidjson/include
CC:= g++
CC_FLAGS:= -DRAPIDJSON_HAS_STDSTRING=1
SRC_FILES:=$(wildcard $(SRC_DIR)*.cpp)
OBJ_FILES:=$(patsubst $(SRC_DIR), $(OBJ_DIR),$(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SRC_FILES)))
DEFILES:=$(patsubst $(SRC_DIR), $(DEP_DIR),$(patsubst $(SRC_DIR)%.cpp,$(DEP_DIR)%.o,$(SRC_FILES)))
LINK_LIBS:= -lpng -lqrencode -lpistache -L$(LIB_DIR)

BINARY_TARGET := aquabox
.PHONY: all
all: binary

.PHONY: binary
binary: $(OBJ_FILES)
	$(CC) $(CC_FLAGS) $(OBJ_FILES) $(LINK_LIBS) -o $(BIN_DIR)$(BINARY_TARGET)

.PHONY: run
run: binary
	$(BIN_DIR)$(BINARY_TARGET)
$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	$(CC) $(CC_FLAGS) -c $< -I$(INC_DIR) -o $@
$(DEP_DIR)%.dep: $(SRC_DIR)%.cpp
	$(CC) $(CC_FLAGS) -MM $< -MT "$@ $(patsubst $(DEP_DIR)%.dep,$(OBJ_DIR)%o,$@)" -I$(INC_DIR) -o $@

-include $(DEP_FILES)

.PHONY: prepare
prepare:
	mkdir -p $(DEP_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(LIB_DIR)

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)*
	rm -rf $(OBJ_DIR)*
	rm -rf $(DEP_DIR)*
