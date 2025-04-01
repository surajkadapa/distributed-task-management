#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

// Template for message types
template <typename T>
class MessageQueue {
private:
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cv;
    bool closed;
    
public:
    MessageQueue() : closed(false) {}
    
    // Close the queue, no more messages will be accepted
    void close() {
        std::lock_guard<std::mutex> lock(mtx);
        closed = true;
        cv.notify_all();
    }
    
    // Check if queue is closed
    bool isClosed() const {
        std::lock_guard<std::mutex> lock(mtx);
        return closed;
    }
    
    // Add message to queue
    bool send(const T& message) {
        std::lock_guard<std::mutex> lock(mtx);
        if (closed) return false;
        
        queue.push(message);
        cv.notify_one();
        return true;
    }
    
    bool receive(T& message, bool wait = true) {
        std::unique_lock<std::mutex> lock(mtx);
        
        if (wait) {
            cv.wait(lock, [this] { return !queue.empty() || closed; });
        }
        
        if (queue.empty()) return false;
        
        message = queue.front();
        queue.pop();
        return true;
    }
    
    // Check if queue is empty
    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.empty();
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.size();
    }
};

// Specialization for string messages
using StringMessageQueue = MessageQueue<std::string>;

#endif // MESSAGE_QUEUE_H
