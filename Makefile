TARGET?=native

BUILD_DIR:=./build
SRC_DIRS:=./src

SRCS:=$(shell find $(SRC_DIRS) -name '*.c')
OBJS:=$(SRCS:%=$(BUILD_DIR)/%.o)
DEPS:=$(OBJS:.o=.d)

INC_DIRS:=./include $(shell find $(SRC_DIRS) -type d)
INC_FLAGS:=$(addprefix -I,$(INC_DIRS))

LDFLAGS:=-lglfw -lGL -lm
CFLAGS:=$(INC_FLAGS) -std=c17 -Wall -Wpedantic -Werror -O2 -MMD -MP -g

ifeq ($(TARGET),web)
	CC:=emcc
	LDFLAGS:=$(LDFLAGS) -O2 --emrun -s USE_GLFW=3 -s FULL_ES3
	TARGET_EXEC=vestige.html
else
	CC:=clang
	TARGET_EXEC=vestige
endif

.PHONY: clean

all: $(BUILD_DIR)/$(TARGET_EXEC)

run: $(BUILD_DIR)/$(TARGET_EXEC)
ifeq ($(TARGET),web)
	emrun $(BUILD_DIR)/$(TARGET_EXEC)
else
	./$(BUILD_DIR)/$(TARGET_EXEC)
endif

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
