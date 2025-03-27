#include "messagequeue.h"

void MessageQueue::enqueueTask(const Task& task) {
    std::lock_guard<std::mutex> lock(mtx);
    queue.push(task);
    cv.notify_one();
}

Task MessageQueue::dequeueTask() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this] { return !queue.empty(); });
    Task task = queue.front();
    queue.pop();
    return task;
}
