// src/taskhistory.cpp
#include "../include/taskhistory.h"

TaskStateMemento::TaskStateMemento(TaskStatus status, const std::string& changedBy, const std::string& comments)
    : status(status), timestamp(std::time(nullptr)), changedBy(changedBy), comments(comments) {
}

TaskStatus TaskStateMemento::getStatus() const {
    return status;
}

time_t TaskStateMemento::getTimestamp() const {
    return timestamp;
}

std::string TaskStateMemento::getChangedBy() const {
    return changedBy;
}

std::string TaskStateMemento::getComments() const {
    return comments;
}

TaskHistory::TaskHistory(int taskId) : taskId(taskId) {
}

void TaskHistory::addState(const TaskStateMemento& state) {
    history.push_back(state);
}

void TaskHistory::addState(TaskStatus status, const std::string& changedBy, const std::string& comments) {
    TaskStateMemento memento(status, changedBy, comments);
    history.push_back(memento);
}

std::vector<TaskStateMemento> TaskHistory::getHistory() const {
    return history;
}

TaskStateMemento TaskHistory::getLatestState() const {
    if (history.empty()) {
        // Return a default state if history is empty
        return TaskStateMemento(TaskStatus::PENDING, "", "");
    }
    
    return history.back();
}

int TaskHistory::getTaskId() const {
    return taskId;
}

int TaskHistory::getStateCount() const {
    return history.size();
}