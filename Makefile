CCPATH := /usr/gcc-5.1.0/bin
CCBIN := g++-5.1.0

CC := $(CCPATH)/$(CCBIN)
RM := rm -rf
MK := mkdir
CP := cp -v

CFLAGS := -c -Wall
LDFLAGS := 

INCLUDE_PATH := 

SOURCE_FILES += hex.cpp

BUILD_DIR := build
RELEASE_DIR := /usr/bin
OUTPUT_BIN := hex

SOURCE_OBJECTS := $(addprefix $(BUILD_DIR)/, $(SOURCE_FILES:.cpp=.o) )

all: clean release

debug: CFLAGS += -DDEBUG -g -O0
debug: clean $(BUILD_DIR)/$(OUTPUT_BIN)

release: CFLAGS += -DNDEBUG -O3
release: $(BUILD_DIR)/$(OUTPUT_BIN)
release:
	sudo $(CP) $(BUILD_DIR)/$(OUTPUT_BIN) $(RELEASE_DIR)/$(OUTPUT_BIN)

$(BUILD_DIR):
	$(MK) $@

$(BUILD_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE_PATH) -M $< -MF $(@:.o=.d) -MT $@
	$(CC) $(CFLAGS) $(INCLUDE_PATH) -c -o $@ $<

$(BUILD_DIR)/$(OUTPUT_BIN): $(BUILD_DIR) $(SOURCE_OBJECTS)
	$(CC) $(LDFLAGS) $(SOURCE_OBJECTS) -o $@

clean:
	$(RM) $(BUILD_DIR)