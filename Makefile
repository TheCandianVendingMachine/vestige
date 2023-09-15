TARGET=vestige

CC=clang

BUILD_DIR:=./build
SRC_DIRS:=./src

SRCS:=$(shell find $(SRC_DIRS) -name '*.c')
OBJS:=$(SRCS:%=$(BUILD_DIR)/%.o)
DEPS:=$(OBJS:.o=.d)

INC_DIRS:=./include $(shell find $(SRC_DIRS) -type d)
INC_FLAGS:=$(addprefix -I,$(INC_DIRS))

CFLAGS:=$(INC_FLAGS) -std=c17 -Wall -Wpedantic -Werror -O2 -MMD -MP

LDFLAGS:=-lglfw -lGL

.PHONY: clean

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
