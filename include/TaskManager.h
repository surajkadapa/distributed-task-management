#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "Task.h"
#include "Node.h"
#include "Scheduler.h"
#include "FIFOScheduler.h"
#include "RoundRobinScheduler.h"
#include "LoadBalancedScheduler.h"
#include <memory>
#include <mutex>
#include <vector>
#include <string>

enum class SchedulerType { 
    FIFO, 
    RoundRobin, 
    LoadBalanced 
};

class TaskManager {
public:
    TaskManager(std::unique_ptr<Scheduler> scheduler);
    ~TaskManager();

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
    
    // Task assignment
    bool assignTaskToNode(int taskId, int nodeId);
    
    // Task status updates
    bool cancelTask(int taskId);
    bool pauseTask(int taskId);
    bool resumeTask(int taskId);

private:
    std::vector<std::shared_ptr<Task>> tasks;
    std::vector<std::shared_ptr<Node>> nodes;
    std::unique_ptr<Scheduler> scheduler;
    SchedulerType currentSchedulerType;

    mutable std::mutex mtx;
    int nextTaskId;
    int nextNodeId;
};

#endif