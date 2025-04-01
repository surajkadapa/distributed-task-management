// include/taskhistory.h
#ifndef TASK_HISTORY_H
#define TASK_HISTORY_H

#include "task.h"
#include <vector>
#include <string>
#include <ctime>

// Memento Pattern
class TaskStateMemento {
private:
    TaskStatus status;
    time_t timestamp;
    std::string changedBy;
    std::string comments;
    
public:
    TaskStateMemento(TaskStatus status, const std::string& changedBy, const std::string& comments);
    
    // Getters
    TaskStatus getStatus() const;
    time_t getTimestamp() const;
    std::string getChangedBy() const;
    std::string getComments() const;
};

class TaskHistory {
private:
    int taskId;
    std::vector<TaskStateMemento> history;
    
public:
    TaskHistory(int taskId);
    
    // History operations
    void addState(const TaskStateMemento& state);
    void addState(TaskStatus status, const std::string& changedBy, const std::string& comments);
    std::vector<TaskStateMemento> getHistory() const;
    TaskStateMemento getLatestState() const;
    
    // Utility
    int getTaskId() const;
    int getStateCount() const;
};

#endif // TASK_HISTORY_H