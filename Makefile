CXX = g++
CXXFLAGS = -std=c++11 -Wall
SRC = src/task.cpp src/workernode.cpp src/taskmanager.cpp main.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = task_manager

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)
