CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -MMD -MP
BUILD_DIR = build
SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*.cpp) main.cpp
OBJ = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRC))
DEP = $(OBJ:.o=.d)
TARGET = task_manager

# Default target
all: $(TARGET)

# Linking the final binary
$(TARGET): $(OBJ)
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compiling source files
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)/src

# Clean build files
clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR) $(TARGET)

# Debug build with debugging symbols
debug: CXXFLAGS += -g
debug: clean all

# Include dependency files
-include $(DEP)
