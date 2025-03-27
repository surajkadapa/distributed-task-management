#include "taskmanager.h"
#include <iostream>

void TaskManager::scheduleTask(Task& task) {
    tasks.push_back(task);
}

void TaskManager::distributeTask(Task& task) {
    if (!workerNodes.empty()) {
        workerNodes[0].executeTask(task);
    }
}

void TaskManager::monitorTasks() {
    std::cout << "Monitoring tasks..." << std::endl;
}

void TaskManager::retryFailedTasks() {
    std::cout << "Retrying failed tasks..." << std::endl;
}

void TaskManager::logTaskActivity() {
    std::cout << "Logging task activities..." << std::endl;
}

void TaskManager::getTaskHistory() {
    std::cout << "Fetching task history..." << std::endl;
}
