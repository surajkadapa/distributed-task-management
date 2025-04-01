// src/monitor.cpp
#include "../include/monitor.h"
#include "../include/loggingservice.h"

Monitor::Monitor(int id, const std::string& name)
    : monitorId(id), name(name) {
}

void Monitor::update(const std::string& message) {
    std::string logMessage = "Monitor '" + name + "' received: " + message;
    logs.push_back(logMessage);
    LoggingService::getInstance()->debug(logMessage);
}

void Monitor::checkNodeStatus(const WorkerNode& node) {
    std::string status = node.isActive() ? "active" : "inactive";
    std::string message = "Node '" + node.getName() + "' is " + status + 
                         " with " + std::to_string(node.getQueueSize()) + " tasks in queue";
    logs.push_back(message);
    LoggingService::getInstance()->info(message);
}

std::vector<std::string> Monitor::getLogs() const {
    return logs;
}

void Monitor::clearLogs() {
    logs.clear();
    LoggingService::getInstance()->debug("Monitor '" + name + "' logs cleared");
}

int Monitor::getId() const {
    return monitorId;
}

std::string Monitor::getName() const {
    return name;
}