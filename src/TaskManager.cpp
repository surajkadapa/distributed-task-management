#include "../include/TaskManager.h"
#include <sstream>
#include <iostream>
#include <algorithm>

TaskManager::TaskManager(std::unique_ptr<Scheduler> scheduler)
    : scheduler(std::move(scheduler)), 
      currentSchedulerType(SchedulerType::FIFO), // Default to FIFO
      nextTaskId(1), 
      nextNodeId(1) {}

TaskManager::~TaskManager() {
    // Stop all nodes when the manager is destroyed
    for (auto& node : nodes) {
        node->stop();
    }
}

void TaskManager::addTask(const std::string& name, int duration) {
    std::lock_guard<std::mutex> lock(mtx);
    
    auto task = std::make_shared<Task>(nextTaskId++, name, duration);
    tasks.push_back(task);
    
    // Try to assign the task to a node immediately
    int nodeIndex = scheduler->pickNode(nodes);
    
    if (nodeIndex != -1) {
        nodes[nodeIndex]->addTask(task);
        std::cout << "Assigned task '" << name << "' to Node " << nodes[nodeIndex]->getId() << std::endl;
    } else {
        std::cout << "No available nodes for task '" << name << "' - task will remain pending\n";
    }
}

void TaskManager::addNode() {
    std::lock_guard<std::mutex> lock(mtx);
    auto node = std::make_shared<Node>(nextNodeId++);
    node->start();
    nodes.push_back(node);
    
    // After adding a new node, check if there are any pending tasks that can be assigned
    for (auto& task : tasks) {
        if (task->getStatus() == TaskStatus::Pending) {
            // Use the scheduler to pick a node
            int nodeIndex = scheduler->pickNode(nodes);
            if (nodeIndex != -1) {
                nodes[nodeIndex]->addTask(task);
                std::cout << "Reassigned pending task '" << task->getName() 
                          << "' to Node " << nodes[nodeIndex]->getId() << std::endl;
                break; // Assign one task at a time to avoid overloading the new node
            }
        }
    }
}

void TaskManager::removeNode(int id) {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        if ((*it)->getId() == id) {
            // Get any pending tasks from this node before stopping it
            auto nodeTasks = (*it)->getTaskQueueSnapshot();
            
            // Stop the node
            (*it)->stop();
            nodes.erase(it);
            
            // Reassign pending tasks to other nodes
            for (auto& task : nodeTasks) {
                if (task->getStatus() == TaskStatus::Pending) {
                    int nodeIndex = scheduler->pickNode(nodes);
                    if (nodeIndex != -1) {
                        nodes[nodeIndex]->addTask(task);
                        std::cout << "Reassigned task from removed node '" << task->getName() 
                                  << "' to Node " << nodes[nodeIndex]->getId() << std::endl;
                    } else {
                        std::cout << "No available nodes for reassigning task '" 
                                  << task->getName() << "'\n";
                    }
                }
            }
            
            break;
        }
    }
}

void TaskManager::setScheduler(SchedulerType type) {
    std::lock_guard<std::mutex> lock(mtx);
    
    switch (type) {
        case SchedulerType::FIFO:
            scheduler = std::make_unique<FIFOScheduler>();
            break;
        case SchedulerType::RoundRobin:
            scheduler = std::make_unique<RoundRobinScheduler>();
            break;
        case SchedulerType::LoadBalanced:
            scheduler = std::make_unique<LoadBalancedScheduler>();
            break;
    }
    
    currentSchedulerType = type;
    std::cout << "Scheduler changed to " << getCurrentSchedulerName() << std::endl;
}

SchedulerType TaskManager::getCurrentSchedulerType() const {
    std::lock_guard<std::mutex> lock(mtx);
    return currentSchedulerType;
}

std::string TaskManager::getCurrentSchedulerName() const {
    std::lock_guard<std::mutex> lock(mtx);
    
    switch (currentSchedulerType) {
        case SchedulerType::FIFO:
            return "First-In-First-Out";
        case SchedulerType::RoundRobin:
            return "Round Robin";
        case SchedulerType::LoadBalanced:
            return "Load Balanced";
        default:
            return "Unknown";
    }
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

std::vector<std::shared_ptr<Task>> TaskManager::getAllTasks() const {
    std::lock_guard<std::mutex> lock(mtx);
    return tasks;
}

std::vector<std::shared_ptr<Node>> TaskManager::getAllNodes() const {
    std::lock_guard<std::mutex> lock(mtx);
    return nodes;
}

bool TaskManager::assignTaskToNode(int taskId, int nodeId) {
    std::lock_guard<std::mutex> lock(mtx);
    
    // Find the task
    auto taskIt = std::find_if(tasks.begin(), tasks.end(), 
        [taskId](const auto& task) { return task->getId() == taskId; });
        
    if (taskIt == tasks.end() || (*taskIt)->getStatus() != TaskStatus::Pending) {
        return false; // Task not found or not pending
    }
    
    // Find the node
    auto nodeIt = std::find_if(nodes.begin(), nodes.end(),
        [nodeId](const auto& node) { return node->getId() == nodeId; });
        
    if (nodeIt == nodes.end() || (*nodeIt)->isBusy()) {
        return false; // Node not found or busy
    }
    
    // Assign the task
    (*nodeIt)->addTask(*taskIt);
    std::cout << "Manually assigned task '" << (*taskIt)->getName() 
              << "' to Node " << nodeId << std::endl;
    
    return true;
}

bool TaskManager::cancelTask(int taskId) {
    std::lock_guard<std::mutex> lock(mtx);
    
    // Find the task
    auto taskIt = std::find_if(tasks.begin(), tasks.end(), 
        [taskId](const auto& task) { return task->getId() == taskId; });
        
    if (taskIt == tasks.end() || (*taskIt)->getStatus() == TaskStatus::Completed) {
        return false; // Task not found or already completed
    }
    
    // For now, just mark it as completed
    // In a more sophisticated system, you might want to actually cancel the task
    // and stop it from executing on the node
    (*taskIt)->setStatus(TaskStatus::Completed);
    std::cout << "Canceled task '" << (*taskIt)->getName() << "'" << std::endl;
    
    return true;
}

bool TaskManager::pauseTask(int taskId) {
    // This would require more sophisticated task state management
    // For now, just a placeholder
    std::cout << "Pause functionality not implemented yet" << std::endl;
    return false;
}

bool TaskManager::resumeTask(int taskId) {
    // This would require more sophisticated task state management
    // For now, just a placeholder
    std::cout << "Resume functionality not implemented yet" << std::endl;
    return false;
}