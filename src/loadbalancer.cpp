// src/loadbalancer.cpp
#include "../include/loadbalancer.h"
#include "../include/loggingservice.h"
#include <algorithm>
#include <climits>

// Strategy implementations
RoundRobinStrategy::RoundRobinStrategy() : lastSelectedIndex(-1) {
}

int RoundRobinStrategy::selectWorkerNode(const std::vector<WorkerNode*>& nodes, const Task& task) {
    if (nodes.empty()) return -1;
    
    // Filter active nodes
    std::vector<WorkerNode*> activeNodes;
    for (auto node : nodes) {
        if (node->isActive()) {
            activeNodes.push_back(node);
        }
    }
    
    if (activeNodes.empty()) return -1;
    
    // Simple round-robin selection
    lastSelectedIndex = (lastSelectedIndex + 1) % activeNodes.size();
    return activeNodes[lastSelectedIndex]->getId();
}

int LeastLoadedStrategy::selectWorkerNode(const std::vector<WorkerNode*>& nodes, const Task& task) {
    if (nodes.empty()) return -1;
    
    WorkerNode* selectedNode = nullptr;
    int minLoad = INT_MAX;
    
    for (auto node : nodes) {
        if (node->isActive() && node->getQueueSize() < minLoad) {
            minLoad = node->getQueueSize();
            selectedNode = node;
        }
    }
    
    return selectedNode ? selectedNode->getId() : -1;
}

int PriorityBasedStrategy::selectWorkerNode(const std::vector<WorkerNode*>& nodes, const Task& task) {
    if (nodes.empty()) return -1;
    
    // For high priority tasks, find the least loaded node
    if (task.getPriority() == TaskPriority::HIGH || task.getPriority() == TaskPriority::CRITICAL) {
        WorkerNode* selectedNode = nullptr;
        int minLoad = INT_MAX;
        
        for (auto node : nodes) {
            if (node->isActive() && node->getQueueSize() < minLoad) {
                minLoad = node->getQueueSize();
                selectedNode = node;
            }
        }
        
        return selectedNode ? selectedNode->getId() : -1;
    }
    else {
        // For lower priority tasks, use round-robin
        static int lastIndex = -1;
        
        // Filter active nodes
        std::vector<WorkerNode*> activeNodes;
        for (auto node : nodes) {
            if (node->isActive()) {
                activeNodes.push_back(node);
            }
        }
        
        if (activeNodes.empty()) return -1;
        
        lastIndex = (lastIndex + 1) % activeNodes.size();
        return activeNodes[lastIndex]->getId();
    }
}

// LoadBalancer implementation
LoadBalancer::LoadBalancer() : strategy(new RoundRobinStrategy()) {
}

LoadBalancer::~LoadBalancer() {
    delete strategy;
}

void LoadBalancer::addWorkerNode(WorkerNode* node) {
    workerNodes.push_back(node);
    LoggingService::getInstance()->info("Added worker node: " + node->getName());
}

void LoadBalancer::removeWorkerNode(int nodeId) {
    auto it = std::find_if(workerNodes.begin(), workerNodes.end(),
                         [nodeId](const WorkerNode* node) { return node->getId() == nodeId; });
                         
    if (it != workerNodes.end()) {
        LoggingService::getInstance()->info("Removed worker node: " + (*it)->getName());
        workerNodes.erase(it);
    }
}

void LoadBalancer::setStrategy(LoadBalancingStrategy* newStrategy) {
    if (strategy) {
        delete strategy;
    }
    strategy = newStrategy;
}

bool LoadBalancer::distributeTask(const Task& task) {
    if (workerNodes.empty()) {
        LoggingService::getInstance()->warning("No worker nodes available to distribute task");
        return false;
    }
    
    int selectedNodeId = strategy->selectWorkerNode(workerNodes, task);
    if (selectedNodeId == -1) {
        LoggingService::getInstance()->warning("Failed to select worker node for task");
        return false;
    }
    
    // Find the selected node
    WorkerNode* selectedNode = nullptr;
    for (auto node : workerNodes) {
        if (node->getId() == selectedNodeId) {
            selectedNode = node;
            break;
        }
    }
    
    if (!selectedNode) {
        LoggingService::getInstance()->error("Selected node ID not found");
        return false;
    }
    
    // Add task to the selected node
    selectedNode->addTask(task);
    LoggingService::getInstance()->info("Task #" + std::to_string(task.getId()) + 
                                    " assigned to node " + selectedNode->getName());
    return true;
}

std::vector<WorkerNode*> LoadBalancer::getWorkerNodes() const {
    return workerNodes;
}