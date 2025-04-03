// include/workernode.h
#ifndef WORKER_NODE_H
#define WORKER_NODE_H

#include "task.h"
#include <string>
#include <vector>
#include <queue>
#include <memory>

// Observer Pattern - Subject
class WorkerNode {
private:
    int nodeId;
    std::string nodeName;
    bool active;
    std::queue<std::shared_ptr<Task>> taskQueue;
    std::vector<int> observers; // IDs of observer objects
    
public:
    WorkerNode(int id, const std::string& name);
    
    // Node operations
    void activate();
    void deactivate();
    bool isActive() const;
    
    // Task operations
    void addTask(const Task& task);
    std::shared_ptr<Task> getNextTask();
    bool hasTasksPending() const;
    int getQueueSize() const;
    
    // Observer pattern methods
    void registerObserver(int observerId);
    void removeObserver(int observerId);
    void notifyObservers(const std::string& message);
    
    // Getters/Setters
    int getId() const;
    std::string getName() const;
    void setName(const std::string& name);
};

#endif // WORKER_NODE_H