CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

# Include & Source directories
INCLUDE_DIR = include
SRC_DIR = src
FRONTEND_DIR = frontend

# Additional Wt paths (assuming installed via /usr/local)
WT_INC = /usr/local/include -Iinclude
WT_LIB = /usr/local/lib

# Libraries
LIBS = -lboost_system -lpthread -lsqlite3

# List of all source files
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.cpp=build/%.o)

# Output binary
BACKEND_BIN = bin/taskmaster_backend

# Create build and bin dirs if not present
$(shell mkdir -p build bin)

all: $(BACKEND_BIN)

$(BACKEND_BIN): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -I$(WT_INC) $^ -L$(WT_LIB) $(LIBS) -o $@

build/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -I$(WT_INC) -c $< -o $@

clean:
	rm -rf build/*.o $(BACKEND_BIN)

.PHONY: all clean