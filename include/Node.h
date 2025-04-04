#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <memory>
#include <atomic>
#include "Task.h"


class Node {
    private:
        int id;
        std::atomic<bool> busy;
        std::atomic<bool> running;
        std::thread worker;
        std::queue<std::shared_ptr<Task>> taskQueue;
        mutable std::mutex mtx;
        std::condition_variable cv;
    
        // NEW FIELDS
        int taskCount = 0;
        std::vector<int> taskIDs;
    
    public:
        explicit Node(int id);
    
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
    