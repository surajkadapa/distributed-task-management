CXX = g++
CXXFLAGS = -std=c++11 -Wall
SRC = src/Task.cpp src/WorkerNode.cpp src/TaskManager.cpp main.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = task_manager

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)
