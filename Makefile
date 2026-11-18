CC ?= cc
CPPFLAGS ?= -Isrc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2 -g
LDFLAGS ?=

BUILD_DIR := build
TEST_BIN := $(BUILD_DIR)/test_atlas
SOURCES := src/atlas.c tests/test_atlas.c
HEADERS := src/atlas.h

.PHONY: all test clean

all: test

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TEST_BIN): $(SOURCES) $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $@ $(LDFLAGS)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(BUILD_DIR)
