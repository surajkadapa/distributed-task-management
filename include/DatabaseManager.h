#pragma once
#include <string>
#include <memory>
#include <vector>
#include <sqlite3.h>
#include "Task.h"
#include "Node.h"

// Forward declaration
class TaskManager;

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbPath = "taskmaster.db");
    ~DatabaseManager();
    
    // Database initialization
    bool initialize();
    
    // Task operations
    bool saveTask(const std::shared_ptr<Task>& task);
    bool updateTaskStatus(int taskId, TaskStatus status);
    std::vector<std::shared_ptr<Task>> loadAllTasks();
    std::shared_ptr<Task> loadTask(int taskId);
    bool deleteTask(int taskId);
    
    // Node operations
    bool saveNode(const std::shared_ptr<Node>& node);
    bool updateNodeTaskCount(int nodeId, int taskCount);
    std::vector<std::shared_ptr<Node>> loadAllNodes(TaskManager* manager);
    bool deleteNode(int nodeId);
    
    // Task assignment operations
    bool assignTaskToNode(int taskId, int nodeId);
    bool removeTaskFromNode(int taskId, int nodeId);
    std::vector<int> getNodeTaskIds(int nodeId);
    
    // Statistics/info operations
    int getTaskCount();
    int getNodeCount();
    int getPendingTaskCount();
    int getRunningTaskCount();
    int getCompletedTaskCount();
    
    // Utility functions
    int getLastInsertId();
    int getMaxTaskId();
    int getMaxNodeId();
    
private:
    sqlite3* db;
    std::string dbPath;
    
    // Helper methods for statement preparation and error handling
    sqlite3_stmt* prepareStatement(const std::string& sql);
    void logError(const std::string& operation);
};