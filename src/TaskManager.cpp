#include "../include/TaskManager.h"
#include <sstream>
#include <iostream>

TaskManager::TaskManager(std::unique_ptr<Scheduler> scheduler)
    : scheduler(std::move(scheduler)), nextTaskId(1), nextNodeId(1) {}

    void TaskManager::addTask(const std::string& name, int duration) {
        std::lock_guard<std::mutex> lock(mtx);
    
        auto task = std::make_shared<Task>(nextTaskId++, name, duration);
        int nodeIndex = scheduler->pickNode(nodes);
    
        if (nodeIndex != -1) {
            nodes[nodeIndex]->addTask(task); // Pass shared_ptr
            std::cout << "Assigned task '" << name << "' to Node " << nodes[nodeIndex]->getId() << std::endl;
        } else {
            std::cout << "No available nodes for task '" << name << "'\n";
        }
    
        tasks.push_back(task); // shared_ptr now
    }
    
    

void TaskManager::addNode() {
    std::lock_guard<std::mutex> lock(mtx);
    auto node = std::make_shared<Node>(nextNodeId++);
    node->start();
    nodes.push_back(node);
}

std::vector<std::string> TaskManager::getAllNodesInfo() const {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::string> result;

    for (const auto& node : nodes) {
        std::ostringstream os;
        os << "Node-" << node->getId() << " | Tasks: " << node->getTaskCount();
        result.push_back(os.str());
    }

    return result;
}

void TaskManager::removeNode(int id) {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        if ((*it)->getId() == id) {
            (*it)->stop();
            nodes.erase(it);
            break;
        }
    }
}

std::vector<std::shared_ptr<Task>> TaskManager::getAllTasks() const {
    std::lock_guard<std::mutex> lock(mtx);
    return tasks;
}

std::vector<std::shared_ptr<Node>> TaskManager::getAllNodes() const {
    std::lock_guard<std::mutex> lock(mtx);
    return nodes;
}
