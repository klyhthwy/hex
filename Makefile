CC := clang++
RM := rm -rf
MK := mkdir
CP := cp -v

BUILD_DIR := _build
RELEASE_DIR := /usr/local/bin
OUTPUT_BIN := hex

SOURCE_FILES := hex.cpp

FLAGS := -Weverything -Wno-format-nonliteral -Wno-padded

all: release

debug: FLAGS += -DDEBUG -g -O0
debug: clean $(BUILD_DIR)/$(OUTPUT_BIN)

release: FLAGS += -DNDEBUG -O3
release: clean $(BUILD_DIR)/$(OUTPUT_BIN)

.PHONY: install
install: $(BUILD_DIR)/$(OUTPUT_BIN)
	sudo $(CP) $(BUILD_DIR)/$(OUTPUT_BIN) $(RELEASE_DIR)/$(OUTPUT_BIN)

$(BUILD_DIR):
	$(MK) $@

$(BUILD_DIR)/$(OUTPUT_BIN): $(BUILD_DIR) $(SOURCE_FILES)
	$(CC) $(FLAGS) $(SOURCE_FILES) -o $@

.PHONY: clean
clean:
	$(RM) $(BUILD_DIR)
