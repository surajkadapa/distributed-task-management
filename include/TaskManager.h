#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "Task.h"
#include "Node.h"
#include "Scheduler.h"
#include <memory>
#include <mutex>
#include <vector>
#include <string>

class TaskManager {
public:
    TaskManager(std::unique_ptr<Scheduler> scheduler);

    void addTask(const std::string& name, int duration);
    void addNode();
    void removeNode(int id);

    std::vector<std::shared_ptr<Task>> getAllTasks() const;
    std::vector<std::shared_ptr<Node>> getAllNodes() const;
    std::vector<std::string> getAllNodesInfo() const;

private:
    std::vector<std::shared_ptr<Task>> tasks;
    std::vector<std::shared_ptr<Node>> nodes;
    std::unique_ptr<Scheduler> scheduler;

    mutable std::mutex mtx;
    int nextTaskId;
    int nextNodeId;
};

#endif
