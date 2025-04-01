// src/taskmanager.cpp
#include "../include/taskmanager.h"
#include "../include/loggingservice.h"
#include <algorithm>

// Initialize static instance pointer
TaskManager* TaskManager::instance = nullptr;

// Singleton implementation
TaskManager* TaskManager::getInstance() {
    if (instance == nullptr) {
        instance = new TaskManager();
    }
    return instance;
}

TaskManager::TaskManager() {
    // Initialize with empty task list
}

int TaskManager::createTask(const std::string& title, const std::string& desc, 
                          TaskPriority priority, int assignedUser, int createdBy) {
    // Generate a new task ID (in a real system, this would be handled by a database)
    int newId = tasks.empty() ? 1 : tasks.back().getId() + 1;
    
    // Create the new task
    Task newTask(newId, title, desc, priority, assignedUser, createdBy);
    tasks.push_back(newTask);
    
    // Log the creation
    LoggingService::getInstance()->info("Created task #" + std::to_string(newId) + ": " + title);
    
    return newId;
}

bool TaskManager::updateTask(int taskId, const std::string& title, const std::string& desc) {
    Task* task = getTaskById(taskId);
    if (!task) return false;
    
    task->setTitle(title);
    task->setDescription(desc);
    
    LoggingService::getInstance()->info("Updated task #" + std::to_string(taskId));
    return true;
}

bool TaskManager::deleteTask(int taskId) {
    auto it = std::find_if(tasks.begin(), tasks.end(), 
                         [taskId](const Task& task) { return task.getId() == taskId; });
                         
    if (it == tasks.end()) return false;
    
    tasks.erase(it);
    LoggingService::getInstance()->info("Deleted task #" + std::to_string(taskId));
    return true;
}

bool TaskManager::assignTask(int taskId, int userId) {
    Task* task = getTaskById(taskId);
    if (!task) return false;
    
    task->setAssignedUserId(userId);
    LoggingService::getInstance()->info("Assigned task #" + std::to_string(taskId) + 
                                      " to user #" + std::to_string(userId));
    return true;
}

bool TaskManager::updateTaskStatus(int taskId, TaskStatus status) {
    Task* task = getTaskById(taskId);
    if (!task) return false;
    
    task->setStatus(status);
    LoggingService::getInstance()->info("Updated task #" + std::to_string(taskId) + 
                                      " status to " + task->statusToString());
    return true;
}

Task* TaskManager::getTaskById(int taskId) {
    for (auto& task : tasks) {
        if (task.getId() == taskId) {
            return &task;
        }
    }
    return nullptr;
}

std::vector<Task> TaskManager::getTasksByUser(int userId) {
    std::vector<Task> result;
    for (const auto& task : tasks) {
        if (task.getAssignedUserId() == userId) {
            result.push_back(task);
        }
    }
    return result;
}

std::vector<Task> TaskManager::getTasksByStatus(TaskStatus status) {
    std::vector<Task> result;
    for (const auto& task : tasks) {
        if (task.getStatus() == status) {
            result.push_back(task);
        }
    }
    return result;
}

std::vector<Task> TaskManager::getAllTasks() {
    return tasks;
}