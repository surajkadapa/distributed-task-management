#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "Task.h"
#include <memory>
#include <mutex>
#include <vector>
#include <string>

// Forward declarations
class Node;
class Scheduler;
class FIFOScheduler;
class RoundRobinScheduler;
class LoadBalancedScheduler;
class DatabaseManager;

enum class SchedulerType { 
    FIFO, 
    RoundRobin, 
    LoadBalanced 
};

class TaskManager {
public:
    TaskManager(std::unique_ptr<Scheduler> scheduler, const std::string& dbPath = "taskmaster.db");
    ~TaskManager();

    // Initialization
    bool initialize();

    // Task management
    void addTask(const std::string& name, int duration);
    
    // Node management
    void addNode();
    void removeNode(int id);
    
    // Scheduler management
    void setScheduler(SchedulerType type);
    SchedulerType getCurrentSchedulerType() const;
    std::string getCurrentSchedulerName() const;
    
    // Getters for tasks and nodes
    std::vector<std::shared_ptr<Task>> getAllTasks() const;
    std::vector<std::shared_ptr<Node>> getAllNodes() const;
    std::vector<std::string> getAllNodesInfo() const;
    mutable std::mutex mtx;
    // Task assignment
    bool assignTaskToNode(int taskId, int nodeId);
    std::vector<std::shared_ptr<Task>> tasks;
    std::vector<std::shared_ptr<Node>> nodes;
    std::unique_ptr<Scheduler> scheduler;
    // Task status updates
    bool cancelTask(int taskId);
    bool pauseTask(int taskId);
    bool resumeTask(int taskId);
    
    // Database operations
    std::shared_ptr<DatabaseManager> getDbManager() { return dbManager; }
    
    // Database statistics
    int getTotalTaskCount() const;
    int getPendingTaskCount() const;
    int getRunningTaskCount() const;
    int getCompletedTaskCount() const;
    int getTotalNodeCount() const;

private:
    SchedulerType currentSchedulerType;
    std::string currentSchedulerName;

    int nextTaskId;
    int nextNodeId;
    
    // Database manager
    std::shared_ptr<DatabaseManager> dbManager;
};

#endif