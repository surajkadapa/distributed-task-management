#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <memory>
#include <atomic>
#include "Task.h"

// Forward declarations to break circular dependencies
class TaskManager;

class Node {
private:
    int id;
    std::atomic<bool> busy;
    std::atomic<bool> running;
    std::thread worker;
    std::queue<std::shared_ptr<Task>> taskQueue;
    TaskManager* taskManager;
    mutable std::mutex mtx;
    std::condition_variable cv;
    // NEW FIELDS
    int taskCount = 0;
    std::vector<int> taskIDs;
    
public:
    explicit Node(int id);
    Node(int id, TaskManager* manager);
    void start();
    void stop();
    void addTask(std::shared_ptr<Task> task);
    bool isBusy() const;
    int getId() const;
    int getTaskCount() const;
    std::vector<int> getTaskIDs() const;
    std::vector<std::shared_ptr<Task>> getTaskQueueSnapshot();
    
private:
    void processTasks();
};