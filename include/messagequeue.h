#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include "Task.h"

class MessageQueue {
private:
    std::queue<Task> queue;
    std::mutex mtx;
    std::condition_variable cv;

public:
    void enqueueTask(const Task& task);
    Task dequeueTask();
};

#endif
