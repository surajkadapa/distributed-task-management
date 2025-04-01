// src/scheduler.cpp
#include "../include/scheduler.h"
#include "../include/loggingservice.h"
#include <algorithm>
#include <thread>
#include <chrono>

ExecuteTaskCommand::ExecuteTaskCommand(const Task& task, std::function<void(const Task&)> executor)
    : task(task), executor(executor) {
}

void ExecuteTaskCommand::execute() {
    executor(task);
}

Scheduler::Scheduler() : running(false) {
}

Scheduler::~Scheduler() {
    stop();
    clearAllTasks();
}

void Scheduler::scheduleTask(time_t executionTime, SchedulerCommand* command) {
    scheduledTasks.push(std::make_pair(executionTime, command));
    LoggingService::getInstance()->debug("Task scheduled for " + 
                                       std::to_string(executionTime));
}

void Scheduler::processScheduledTasks() {
    while (running) {
        time_t now = std::time(nullptr);
        bool processed = false;
        
        while (!scheduledTasks.empty() && scheduledTasks.top().first <= now) {
            auto pair = scheduledTasks.top();
            scheduledTasks.pop();
            
            if (pair.second) {
                pair.second->execute();
                delete pair.second;  // Clean up the command
                processed = true;
            }
        }
        
        if (!processed) {
            // Sleep for a short time to prevent CPU spiking
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void Scheduler::start() {
    if (!running) {
        running = true;
        LoggingService::getInstance()->info("Scheduler started");
        
        // In a real system, this would start a thread to process tasks
        // For simplicity, we'll just set the flag here
    }
}

void Scheduler::stop() {
    if (running) {
        running = false;
        LoggingService::getInstance()->info("Scheduler stopped");
    }
}

bool Scheduler::isRunning() const {
    return running;
}

int Scheduler::getTaskCount() const {
    return scheduledTasks.size();
}

void Scheduler::clearAllTasks() {
    // Clean up all scheduled commands
    while (!scheduledTasks.empty()) {
        delete scheduledTasks.top().second;
        scheduledTasks.pop();
    }
    LoggingService::getInstance()->info("All scheduled tasks cleared");
}