// include/taskmanager.h
#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "task.h"
#include <vector>
#include <memory>

// Singleton Pattern implementation
class TaskManager {
private:
    static TaskManager* instance;
    std::vector<Task> tasks;
    
    // Private constructor for Singleton pattern
    TaskManager();

public:
    // Delete copy constructor and assignment operator
    TaskManager(const TaskManager&) = delete;
    TaskManager& operator=(const TaskManager&) = delete;
    
    // Singleton instance accessor
    static TaskManager* getInstance();
    
    // Task operations
    int createTask(const std::string& title, const std::string& desc, 
                 TaskPriority priority, int assignedUser, int createdBy);
    bool updateTask(int taskId, const std::string& title, const std::string& desc);
    bool deleteTask(int taskId);
    bool assignTask(int taskId, int userId);
    bool updateTaskStatus(int taskId, TaskStatus status);
    
    // Task retrieval
    Task* getTaskById(int taskId);
    std::vector<Task> getTasksByUser(int userId);
    std::vector<Task> getTasksByStatus(TaskStatus status);
    std::vector<Task> getAllTasks();
};

#endif // TASK_MANAGER_H