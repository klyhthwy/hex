CC := g++
RM := rm -rf
MK := mkdir
CP := cp -v

BUILD_DIR := _build
DEBUG_DIR := $(BUILD_DIR)/debug
RELEASE_DIR := $(BUILD_DIR)/release
INSTALL_DIR := /usr/local/bin
OUTPUT_BIN := hex.exe

SOURCE_FILES := hex.cpp

FLAGS := -Wall

all: debug release

debug: $(DEBUG_DIR)/$(OUTPUT_BIN)

release: $(RELEASE_DIR)/$(OUTPUT_BIN)

.PHONY: install
install: $(RELEASE_DIR)/$(OUTPUT_BIN)
	$(CP) $(RELEASE_DIR)/$(OUTPUT_BIN) $(INSTALL_DIR)/$(OUTPUT_BIN)

$(BUILD_DIR):
	$(MK) $@

$(DEBUG_DIR): $(BUILD_DIR)
	$(MK) $@

$(RELEASE_DIR): $(BUILD_DIR)
	$(MK) $@

$(DEBUG_DIR)/$(OUTPUT_BIN): $(DEBUG_DIR) $(SOURCE_FILES)
	$(CC) $(FLAGS) -DDEBUG -g -O0 $(SOURCE_FILES) -o $@

$(RELEASE_DIR)/$(OUTPUT_BIN): $(RELEASE_DIR) $(SOURCE_FILES)
	$(CC) $(FLAGS) -DNDEBUG -O3 $(SOURCE_FILES) -o $@

.PHONY: clean
clean:
	$(RM) $(BUILD_DIR)
