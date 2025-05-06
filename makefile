# CVim Makefile

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -O2
DEBUG_FLAGS = -g -DDEBUG

# Check for yaml-cpp
YAML_CPP_LIBS := $(shell pkg-config --libs yaml-cpp 2>/dev/null)
ifdef YAML_CPP_LIBS
    CXXFLAGS += -DHAS_YAML_CPP $(shell pkg-config --cflags yaml-cpp)
    LDFLAGS += $(YAML_CPP_LIBS)
else
    # Optional: Print warning during build
    $(info yaml-cpp not found. Building without YAML support.)
endif

LDFLAGS += -lncurses

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Source files
SOURCES = $(shell find $(SRC_DIR) -name "*.cpp")
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# Main target
TARGET = $(BIN_DIR)/cvim

# Default target
all: $(TARGET)

# Debug target
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: all

# Create directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/modules
	mkdir -p $(BUILD_DIR)/config
	mkdir -p $(BUILD_DIR)/utils

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link the executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Install
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	rm -f /usr/local/bin/cvim

# Run the program
run: $(TARGET)
	$(TARGET)

# Install dependencies
deps:
	@echo "Installing dependencies..."
	@if [ -x "$(command -v apt-get)" ]; then \
		sudo apt-get install -y libncurses5-dev libyaml-cpp-dev; \
	elif [ -x "$(command -v brew)" ]; then \
		brew install ncurses yaml-cpp; \
	else \
		echo "Unable to install dependencies automatically. Please install ncurses and yaml-cpp manually."; \
		exit 1; \
	fi

# Help
help:
	@echo "Available commands:"
	@echo "  make         - Build CVim"
	@echo "  make debug   - Build with debug flags"
	@echo "  make clean   - Remove build files"
	@echo "  make install - Install CVim to /usr/local/bin"
	@echo "  make run     - Build and run CVim"
	@echo "  make deps    - Install dependencies (requires apt or brew)"
	@echo "  make help    - Display this help message"

.PHONY: all debug clean install uninstall run deps help
