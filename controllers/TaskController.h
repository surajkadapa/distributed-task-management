#ifndef TASK_CONTROLLER_H
#define TASK_CONTROLLER_H

#include <memory>
#include <vector>
#include <string>
#include "../models/Task.h"
#include "../models/User.h"
#include "../models/Project.h"
#include "../models/TaskComment.h"
#include "../models/TaskAttachment.h"
#include "../strategies/TaskPriorityStrategy.h"

class TaskController {
private:
    std::shared_ptr<TaskPriorityStrategy> priorityStrategy;

public:
    TaskController();
    
    // Set prioritization strategy (Strategy Pattern)
    void setPriorityStrategy(std::shared_ptr<TaskPriorityStrategy> strategy);
    
    // Task CRUD operations
    std::shared_ptr<Task> createTask(const std::string& type,
                                    const std::string& title,
                                    const std::string& description,
                                    TaskPriority priority,
                                    const std::string& dueDate,
                                    std::shared_ptr<User> creator,
                                    int projectId);
    
    static std::shared_ptr<Task> getTaskById(int taskId);
    std::vector<std::shared_ptr<Task>> getTasksByProject(int projectId);
    std::vector<std::shared_ptr<Task>> getTasksByUser(int userId);
    bool updateTask(std::shared_ptr<Task> task);
    bool deleteTask(int taskId);
    
    // Task assignment
    bool assignTask(int taskId, int userId);
    
    // Task status management
    bool completeTask(int taskId, std::shared_ptr<User> user);
    bool changeTaskStatus(int taskId, TaskStatus status, std::shared_ptr<User> user);
    
    // Task comments
    TaskComment addComment(int taskId, std::shared_ptr<User> user, const std::string& content);
    std::vector<TaskComment> getTaskComments(int taskId);
    
    // Task attachments
    TaskAttachment addAttachment(int taskId, std::shared_ptr<User> user, 
                                const std::string& filename, const std::string& filePath);
    std::vector<TaskAttachment> getTaskAttachments(int taskId);
    
    // Task prioritization using Strategy Pattern
    std::vector<std::shared_ptr<Task>> getPrioritizedTasks(int projectId);
};

#endif // TASK_CONTROLLER_H