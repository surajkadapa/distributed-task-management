// src/workernode.cpp
#include "../include/workernode.h"
#include "../include/loggingservice.h"
#include <algorithm>

WorkerNode::WorkerNode(int id, const std::string& name)
    : nodeId(id), nodeName(name), active(false) {
}

void WorkerNode::activate() {
    active = true;
    LoggingService::getInstance()->info("Worker node '" + nodeName + "' activated");
    notifyObservers("Node activated: " + nodeName);
}

void WorkerNode::deactivate() {
    active = false;
    LoggingService::getInstance()->info("Worker node '" + nodeName + "' deactivated");
    notifyObservers("Node deactivated: " + nodeName);
}

bool WorkerNode::isActive() const {
    return active;
}

void WorkerNode::addTask(const Task& task) {
    taskQueue.push(std::make_shared<Task>(task));
    LoggingService::getInstance()->info("Task #" + std::to_string(task.getId()) + 
                                      " added to node '" + nodeName + "'");
    notifyObservers("Task added to node: " + nodeName);
}

std::shared_ptr<Task> WorkerNode::getNextTask() {
    if (taskQueue.empty()) {
        return nullptr; // Return nullptr if queue is empty
    }

    std::shared_ptr<Task> task = taskQueue.front(); // Get shared_ptr<Task>
    taskQueue.pop();
    
    LoggingService::getInstance()->info("Processing task #" + std::to_string(task->getId()) + 
                                      " on node '" + nodeName + "'");
    
    return task;
}


bool WorkerNode::hasTasksPending() const {
    return !taskQueue.empty();
}

int WorkerNode::getQueueSize() const {
    return taskQueue.size();
}

void WorkerNode::registerObserver(int observerId) {
    observers.push_back(observerId);
    LoggingService::getInstance()->debug("Observer #" + std::to_string(observerId) + 
                                       " registered to node '" + nodeName + "'");
}

void WorkerNode::removeObserver(int observerId) {
    auto it = std::find(observers.begin(), observers.end(), observerId);
    if (it != observers.end()) {
        observers.erase(it);
        LoggingService::getInstance()->debug("Observer #" + std::to_string(observerId) + 
                                           " removed from node '" + nodeName + "'");
    }
}

void WorkerNode::notifyObservers(const std::string& message) {
    // In a real system, this would actually notify the observer objects
    LoggingService::getInstance()->debug("Notifying " + std::to_string(observers.size()) + 
                                       " observers: " + message);
}

int WorkerNode::getId() const {
    return nodeId;
}

std::string WorkerNode::getName() const {
    return nodeName;
}

void WorkerNode::setName(const std::string& name) {
    nodeName = name;
}