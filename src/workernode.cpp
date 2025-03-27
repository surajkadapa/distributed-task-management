#include "workernode.h"
#include <iostream>

WorkerNode::WorkerNode(std::string id, int resources) : nodeID(id), availableResources(resources) {}

void WorkerNode::executeTask(Task& task) {
    task.execute();
    availableResources--;
}

void WorkerNode::reportStatus() {
    std::cout << "WorkerNode " << nodeID << " reporting status." << std::endl;
}

void WorkerNode::heartbeat() {
    std::cout << "WorkerNode " << nodeID << " heartbeat sent." << std::endl;
}

void WorkerNode::recoverTask(Task& task) {
    task.execute();
}
