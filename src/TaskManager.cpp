#include "../include/TaskManager.h"
#include "../include/Node.h"
#include "../include/Scheduler.h"
#include "../include/FIFOScheduler.h"
#include "../include/RoundRobinScheduler.h"
#include "../include/LoadBalancedScheduler.h"
#include <sstream>
#include <iostream>
#include <algorithm>

TaskManager::TaskManager(std::unique_ptr<Scheduler> scheduler)
    : scheduler(std::move(scheduler)), 
      currentSchedulerType(SchedulerType::FIFO), // Default to FIFO
      currentSchedulerName("FIFO"), // Set default name
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
    auto node = std::make_shared<Node>(nextNodeId++, this); 
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
    try {
        std::lock_guard<std::mutex> lock(mtx);
        
        std::cout << "Changing scheduler to type: " << static_cast<int>(type) << std::endl;
        
        // Create the new scheduler
        std::unique_ptr<Scheduler> newScheduler;
        switch (type) {
            case SchedulerType::FIFO:
                std::cout << "Creating FIFO scheduler" << std::endl;
                newScheduler = std::make_unique<FIFOScheduler>();
                currentSchedulerName = "FIFO";
                break;
            case SchedulerType::RoundRobin:
                std::cout << "Creating RoundRobin scheduler" << std::endl;
                newScheduler = std::make_unique<RoundRobinScheduler>();
                currentSchedulerName = "RoundRobin";
                break;
            case SchedulerType::LoadBalanced:
                std::cout << "Creating LoadBalanced scheduler" << std::endl;
                newScheduler = std::make_unique<LoadBalancedScheduler>();
                currentSchedulerName = "LoadBalanced";
                break;
            default:
                std::cout << "Unknown scheduler type, defaulting to FIFO" << std::endl;
                newScheduler = std::make_unique<FIFOScheduler>();
                currentSchedulerName = "FIFO";
                type = SchedulerType::FIFO;
                break;
        }

        
        // Now assign the new scheduler
        scheduler = std::move(newScheduler);
        currentSchedulerType = type;
        std::cout << "Scheduler successfully changed to " << getCurrentSchedulerName() << std::endl;
    } 
    catch (const std::exception& e) {
        std::cerr << "Exception in setScheduler: " << e.what() << std::endl;
        // Fallback to FIFO scheduler in case of error
        scheduler = std::make_unique<FIFOScheduler>();
        currentSchedulerType = SchedulerType::FIFO;
    } 
    catch (...) {
        std::cerr << "Unknown exception in setScheduler" << std::endl;
        // Fallback to FIFO scheduler in case of error
        scheduler = std::make_unique<FIFOScheduler>();
        currentSchedulerType = SchedulerType::FIFO;
    }
}


SchedulerType TaskManager::getCurrentSchedulerType() const {
    std::lock_guard<std::mutex> lock(mtx);
    return currentSchedulerType;
}

std::string TaskManager::getCurrentSchedulerName() const {
    return currentSchedulerName;
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