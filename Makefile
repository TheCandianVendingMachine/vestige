TARGET?=native

BUILD_DIR:=./build
SRC_DIRS:=./src
TEST_DIR:=./test

BUILD_FILE:=./src/vestige.c
TEST_FILE:=./test/main.c

SRCS:=$(shell find $(SRC_DIRS) -name '*.c' ! -wholename $(BUILD_FILE))
ENGINE_OBJS:=$(SRCS:%=$(BUILD_DIR)/%.o)
OBJS:=$(BUILD_FILE:%=$(BUILD_DIR)/%.o) $(ENGINE_OBJS)
DEPS:=$(OBJS:.o=.d)

TEST_SRCS:=$(shell find $(TEST_DIR) -name '*.c' ! -wholename $(TEST_FILE))
TEST_OBJS:=$(TEST_SRCS:%=$(BUILD_DIR)/%.o) $(TEST_FILE:%=$(BUILD_DIR)/%.o)
TEST_DEPS:=$(TEST_OBJS:.o=.d)

INC_DIRS:=./include $(shell find $(SRC_DIRS) -type d) ./ext
INC_FLAGS:=$(addprefix -I,$(INC_DIRS)) `pkg-config --cflags freetype2`

LDFLAGS:=-lglfw -lGL -lm `pkg-config --libs --static freetype2`
CFLAGS:=-std=c17 -Wall -Wpedantic -Werror -O0 -MMD -MP -ggdb -pthread

ifeq ($(TARGET),web)
	CC:=emcc
	LDFLAGS:=$(LDFLAGS) -O2 --emrun -s USE_GLFW=3 -s FULL_ES3
	TARGET_EXEC=vestige.html
	TEST_TARGET_EXEC=vestige_tests.html
else
	CC:=clang
	TARGET_EXEC=vestige
	TEST_TARGET_EXEC=vestige_tests
endif

.PHONY: clean check distcheck test

all: $(BUILD_DIR)/$(TARGET_EXEC)

run: all
ifeq ($(TARGET),web)
	emrun $(BUILD_DIR)/$(TARGET_EXEC)
else
	./$(BUILD_DIR)/$(TARGET_EXEC)
endif

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/$(TEST_TARGET_EXEC): $(ENGINE_OBJS) $(TEST_OBJS)
	$(CC) $(LDFLAGS) $(TEST_OBJS) $(ENGINE_OBJS) -o $@

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

check:
	@echo "Checked!"

clean:
	rm -rf $(BUILD_DIR)

test: $(BUILD_DIR)/$(TEST_TARGET_EXEC)
	./$(BUILD_DIR)/$(TEST_TARGET_EXEC)

-include $(DEPS)
-include $(TEST_DEPS)
