// src/task.cpp
#include "../include/task.h"
#include <ctime>

Task::Task() : taskId(0), priority(TaskPriority::MEDIUM), status(TaskStatus::PENDING),
               creationTime(std::time(nullptr)), dueTime(0), assignedUserId(0), createdByUserId(0) {
}

Task::Task(int id, const std::string& title, const std::string& desc, 
          TaskPriority priority, int assignedUser, int createdBy) 
    : taskId(id), title(title), description(desc), priority(priority), 
      status(TaskStatus::PENDING), creationTime(std::time(nullptr)), 
      dueTime(0), assignedUserId(assignedUser), createdByUserId(createdBy) {
}

// Getters implementation
int Task::getId() const { return taskId; }
std::string Task::getTitle() const { return title; }
std::string Task::getDescription() const { return description; }
TaskPriority Task::getPriority() const { return priority; }
TaskStatus Task::getStatus() const { return status; }
time_t Task::getCreationTime() const { return creationTime; }
time_t Task::getDueTime() const { return dueTime; }
int Task::getAssignedUserId() const { return assignedUserId; }
int Task::getCreatedByUserId() const { return createdByUserId; }

// Setters implementation
void Task::setTitle(const std::string& newTitle) { title = newTitle; }
void Task::setDescription(const std::string& desc) { description = desc; }
void Task::setPriority(TaskPriority newPriority) { priority = newPriority; }
void Task::setStatus(TaskStatus newStatus) { status = newStatus; }
void Task::setDueTime(time_t newDueTime) { dueTime = newDueTime; }
void Task::setAssignedUserId(int userId) { assignedUserId = userId; }

// Utility methods
std::string Task::priorityToString() const {
    switch (priority) {
        case TaskPriority::LOW: return "Low";
        case TaskPriority::MEDIUM: return "Medium";
        case TaskPriority::HIGH: return "High";
        case TaskPriority::CRITICAL: return "Critical";
        default: return "Unknown";
    }
}

std::string Task::statusToString() const {
    switch (status) {
        case TaskStatus::PENDING: return "Pending";
        case TaskStatus::IN_PROGRESS: return "In Progress";
        case TaskStatus::COMPLETED: return "Completed";
        case TaskStatus::FAILED: return "Failed";
        default: return "Unknown";
    }
}