// src/main.cpp
#include "/Users/gaganphadke/OOAD/Project/distributed-task-management/include/task.h"
#include "/Users/gaganphadke/OOAD/Project/distributed-task-management/include/user.h"
#include "/Users/gaganphadke/OOAD/Project/distributed-task-management/include/taskmanager.h"
#include "/Users/gaganphadke/OOAD/Project/distributed-task-management/include/workernode.h"
#include "/Users/gaganphadke/OOAD/Project/distributed-task-management/include/monitor.h"
#include "/Users/gaganphadke/OOAD/Project/distributed-task-management/include/loadbalancer.h"
#include "/Users/gaganphadke/OOAD/Project/distributed-task-management/include/scheduler.h"
#include "/Users/gaganphadke/OOAD/Project/distributed-task-management/include/loggingservice.h"
#include "/Users/gaganphadke/OOAD/Project/distributed-task-management/include/taskhistory.h"
#include "/Users/gaganphadke/OOAD/Project/distributed-task-management/include/messagequeue.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <mutex>
#include <memory>
#include <iomanip> // For std::put_time
#include <map>
class Monitor;

// View layer - Simple Command Line Interface
class TaskManagementView {
public:
    void showMainMenu() {
        std::cout << "\n===== Distributed Task Management System =====\n";
        std::cout << "1. Task Management\n";
        std::cout << "2. Worker Node Management\n";
        std::cout << "3. System Monitoring\n";
        std::cout << "4. Task Distribution\n";
        std::cout << "5. View Logs\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter choice: ";
    }
    
    void showTaskMenu() {
        std::cout << "\n===== Task Management =====\n";
        std::cout << "1. Create New Task\n";
        std::cout << "2. View All Tasks\n";
        std::cout << "3. Update Task\n";
        std::cout << "4. Delete Task\n";
        std::cout << "5. Change Task Status\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Enter choice: ";
    }
    
    void showNodeMenu() {
        std::cout << "\n===== Worker Node Management =====\n";
        std::cout << "1. Add New Worker Node\n";
        std::cout << "2. View All Worker Nodes\n";
        std::cout << "3. Activate Node\n";
        std::cout << "4. Deactivate Node\n";
        std::cout << "5. Remove Node\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Enter choice: ";
    }
    
    void showMonitoringMenu() {
        std::cout << "\n===== System Monitoring =====\n";
        std::cout << "1. View Node Status\n";
        std::cout << "2. View Task Queue Sizes\n";
        std::cout << "3. View Task Histories\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Enter choice: ";
    }
    
    void showDistributionMenu() {
        std::cout << "\n===== Task Distribution =====\n";
        std::cout << "1. Change Load Balancing Strategy\n";
        std::cout << "2. Distribute Pending Tasks\n";
        std::cout << "3. View Task Distribution\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Enter choice: ";
    }
    
    void displayTasks(const std::vector<Task>& tasks) {
        if (tasks.empty()) {
            std::cout << "No tasks found.\n";
            return;
        }
        
        std::cout << "\n===== Task List =====\n";
        std::cout << "ID\tTitle\t\tPriority\tStatus\tAssigned To\n";
        std::cout << "--------------------------------------------------------\n";
        
        for (const auto& task : tasks) {
            std::cout << task.getId() << "\t" 
                     << task.getTitle() << "\t\t" 
                     << task.priorityToString() << "\t\t"
                     << task.statusToString() << "\t"
                     << task.getAssignedUserId() << "\n";
        }
    }
    
    void displayWorkerNodes(const std::vector<std::shared_ptr<WorkerNode>>& nodes) {
        if (nodes.empty()) {
            std::cout << "No worker nodes found.\n";
            return;
        }
        
        std::cout << "\n===== Worker Node List =====\n";
        std::cout << "ID\tName\t\tStatus\tQueue Size\n";
        std::cout << "--------------------------------------------------------\n";
        
        for (const auto& node : nodes) {
            std::cout << node->getId() << "\t" 
                     << node->getName() << "\t\t" 
                     << (node->isActive() ? "Active" : "Inactive") << "\t"
                     << node->getQueueSize() << "\n";
        }
    }
    
    void displayLogs(const std::vector<std::string>& logs) {
        if (logs.empty()) {
            std::cout << "No logs found.\n";
            return;
        }
        
        std::cout << "\n===== Recent Logs =====\n";
        for (const auto& log : logs) {
            std::cout << log << "\n";
        }
    }
    
    int getIntInput(const std::string& prompt) {
        std::string input;
        int value;
        
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, input);
            
            try {
                value = std::stoi(input);
                return value;
            }
            catch (const std::exception&) {
                std::cout << "Invalid input. Please enter a number.\n";
            }
        }
    }
    
    std::string getStringInput(const std::string& prompt) {
        std::string input;
        std::cout << prompt;
        std::getline(std::cin, input);
        return input;
    }
};

// Controller layer - Main application controller
class TaskManagementController {
private:
    TaskManagementView view;
    TaskManager* taskManager;
    LoadBalancer loadBalancer;
    std::vector<std::shared_ptr<WorkerNode>> workerNodes;
    std::vector<std::shared_ptr<Monitor>> monitors;
    Scheduler scheduler;
    LoggingService* logger;
    std::map<int, std::shared_ptr<TaskHistory>> taskHistories;
    std::atomic<bool> running;
    std::vector<std::thread> nodeThreads;
    std::mutex taskMutex;
    
public:
    TaskManagementController() {
        taskManager = TaskManager::getInstance();
        logger = LoggingService::getInstance();
        logger->setMinLogLevel(LogLevel::DEBUG);
        running = true;
        
        // Create a monitor
        auto systemMonitor = std::make_shared<Monitor>(1, "System Monitor");
        monitors.push_back(systemMonitor);
        
        // Set up load balancer with default strategy
        loadBalancer.setStrategy(std::make_shared<LeastLoadedStrategy>().get());
        
        logger->info("Task Management System initialized");
    }
    
    ~TaskManagementController() {
        // Signal threads to stop
        running = false;
        
        // Join all worker threads
        for (auto& thread : nodeThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        // Clear resources (shared_ptr will handle deallocation)
        workerNodes.clear();
        monitors.clear();
        taskHistories.clear();
        
        logger->info("Task Management Controller shut down");
    }
    
    void run() {
        while (running) {
            try {
                view.showMainMenu();
                int choice = view.getIntInput("");
                
                switch (choice) {
                    case 1:
                        handleTaskManagement();
                        break;
                    case 2:
                        handleNodeManagement();
                        break;
                    case 3:
                        handleSystemMonitoring();
                        break;
                    case 4:
                        handleTaskDistribution();
                        break;
                    case 5:
                        handleViewLogs();
                        break;
                    case 0:
                        running = false;
                        logger->info("System shutdown initiated");
                        break;
                    default:
                        std::cout << "Invalid choice. Please try again.\n";
                }
            }
            catch (const std::exception& e) {
                logger->error("Exception in main loop: " + std::string(e.what()));
                std::cout << "An error occurred: " << e.what() << std::endl;
            }
        }
        
        std::cout << "Thank you for using the Distributed Task Management System!\n";
    }
    
private:
    void handleTaskManagement() {
        bool taskMenuRunning = true;
        
        while (taskMenuRunning && running) {
            view.showTaskMenu();
            int choice = view.getIntInput("");
            
            switch (choice) {
                case 1:
                    createTask();
                    break;
                case 2:
                    viewAllTasks();
                    break;
                case 3:
                    updateTask();
                    break;
                case 4:
                    deleteTask();
                    break;
                case 5:
                    changeTaskStatus();
                    break;
                case 0:
                    taskMenuRunning = false;
                    break;
                default:
                    std::cout << "Invalid choice. Please try again.\n";
            }
        }
    }
    
    void handleNodeManagement() {
        bool nodeMenuRunning = true;
        
        while (nodeMenuRunning && running) {
            view.showNodeMenu();
            int choice = view.getIntInput("");
            
            switch (choice) {
                case 1:
                    addWorkerNode();
                    break;
                case 2:
                    viewAllNodes();
                    break;
                case 3:
                    activateNode();
                    break;
                case 4:
                    deactivateNode();
                    break;
                case 5:
                    removeNode();
                    break;
                case 0:
                    nodeMenuRunning = false;
                    break;
                default:
                    std::cout << "Invalid choice. Please try again.\n";
            }
        }
    }
    
    void handleSystemMonitoring() {
        bool monitoringMenuRunning = true;
        
        while (monitoringMenuRunning && running) {
            view.showMonitoringMenu();
            int choice = view.getIntInput("");
            
            switch (choice) {
                case 1:
                    viewNodeStatus();
                    break;
                case 2:
                    viewTaskQueueSizes();
                    break;
                case 3:
                    viewTaskHistories();
                    break;
                case 0:
                    monitoringMenuRunning = false;
                    break;
                default:
                    std::cout << "Invalid choice. Please try again.\n";
            }
        }
    }
    
    void handleTaskDistribution() {
        bool distributionMenuRunning = true;
        
        while (distributionMenuRunning && running) {
            view.showDistributionMenu();
            int choice = view.getIntInput("");
            
            switch (choice) {
                case 1:
                    changeLoadBalancingStrategy();
                    break;
                case 2:
                    distributePendingTasks();
                    break;
                case 3:
                    viewTaskDistribution();
                    break;
                case 0:
                    distributionMenuRunning = false;
                    break;
                default:
                    std::cout << "Invalid choice. Please try again.\n";
            }
        }
    }
    
    void handleViewLogs() {
        int count = view.getIntInput("Enter number of recent logs to view: ");
        view.displayLogs(logger->getRecentLogs(count));
    }
    
    // Task Management Methods
    void createTask() {
        try {
            std::string title = view.getStringInput("Enter task title: ");
            std::string description = view.getStringInput("Enter task description: ");
            
            std::cout << "Select priority:\n";
            std::cout << "1. Low\n";
            std::cout << "2. Medium\n";
            std::cout << "3. High\n";
            std::cout << "4. Critical\n";
            int priorityChoice = view.getIntInput("Enter choice: ");
            
            TaskPriority priority;
            switch (priorityChoice) {
                case 1: priority = TaskPriority::LOW; break;
                case 2: priority = TaskPriority::MEDIUM; break;
                case 3: priority = TaskPriority::HIGH; break;
                case 4: priority = TaskPriority::CRITICAL; break;
                default: priority = TaskPriority::MEDIUM;
            }
            
            int assignedUser = view.getIntInput("Enter assigned user ID: ");
            int createdBy = view.getIntInput("Enter creator user ID: ");
            
            std::lock_guard<std::mutex> lock(taskMutex);
            int taskId = taskManager->createTask(title, description, priority, assignedUser, createdBy);
            
            // Create task history
            auto history = std::make_shared<TaskHistory>(taskId);
            history->addState(TaskStatus::PENDING, std::to_string(createdBy), "Task created");
            taskHistories[taskId] = history;
            
            std::cout << "Task created with ID: " << taskId << "\n";
            logger->info("Task created: " + title + " (ID: " + std::to_string(taskId) + ")");
        }
        catch (const std::exception& e) {
            logger->error("Error creating task: " + std::string(e.what()));
            std::cout << "Failed to create task: " << e.what() << std::endl;
        }
    }
    
    void viewAllTasks() {
        try {
            std::lock_guard<std::mutex> lock(taskMutex);
            view.displayTasks(taskManager->getAllTasks());
        }
        catch (const std::exception& e) {
            logger->error("Error viewing tasks: " + std::string(e.what()));
            std::cout << "Failed to view tasks: " << e.what() << std::endl;
        }
    }
    
    void updateTask() {
        try {
            int taskId = view.getIntInput("Enter task ID to update: ");
            
            std::lock_guard<std::mutex> lock(taskMutex);
            Task* taskPtr = taskManager->getTaskById(taskId);
            std::shared_ptr<Task> task(taskPtr, [](Task*) {});
            
            if (!task) {
                std::cout << "Task not found.\n";
                return;
            }
            
            std::cout << "Current task: " << task->getTitle() << " - " << task->getDescription() << "\n";
            
            std::string title = view.getStringInput("Enter new title (leave empty to keep current): ");
            if (title.empty()) {
                title = task->getTitle();
            }
            
            std::string description = view.getStringInput("Enter new description (leave empty to keep current): ");
            if (description.empty()) {
                description = task->getDescription();
            }
            
            if (taskManager->updateTask(taskId, title, description)) {
                std::cout << "Task updated successfully.\n";
                
                // Update task history
                auto it = taskHistories.find(taskId);
                if (it != taskHistories.end()) {
                    it->second->addState(task->getStatus(), "System", "Task details updated");
                }
                logger->info("Task updated: ID " + std::to_string(taskId));
            } else {
                std::cout << "Failed to update task.\n";
                logger->warning("Failed to update task: ID " + std::to_string(taskId));
            }
        }
        catch (const std::exception& e) {
            logger->error("Error updating task: " + std::string(e.what()));
            std::cout << "Failed to update task: " << e.what() << std::endl;
        }
    }
    
    void deleteTask() {
        try {
            int taskId = view.getIntInput("Enter task ID to delete: ");
            
            std::lock_guard<std::mutex> lock(taskMutex);
            if (taskManager->deleteTask(taskId)) {
                std::cout << "Task deleted successfully.\n";
                
                // Remove task history
                auto it = taskHistories.find(taskId);
                if (it != taskHistories.end()) {
                    taskHistories.erase(it);
                }
                logger->info("Task deleted: ID " + std::to_string(taskId));
            } else {
                std::cout << "Failed to delete task. Task not found.\n";
                logger->warning("Failed to delete task: ID " + std::to_string(taskId) + " - not found");
            }
        }
        catch (const std::exception& e) {
            logger->error("Error deleting task: " + std::string(e.what()));
            std::cout << "Failed to delete task: " << e.what() << std::endl;
        }
    }
    
    void changeTaskStatus() {
        try {
            int taskId = view.getIntInput("Enter task ID: ");
            
            std::lock_guard<std::mutex> lock(taskMutex);
            Task* taskPtr = taskManager->getTaskById(taskId);
            std::shared_ptr<Task> task(taskPtr, [](Task*) {});
            
            if (!task) {
                std::cout << "Task not found.\n";
                return;
            }
            
            std::cout << "Current status: " << task->statusToString() << "\n";
            std::cout << "Select new status:\n";
            std::cout << "1. Pending\n";
            std::cout << "2. In Progress\n";
            std::cout << "3. Completed\n";
            std::cout << "4. Failed\n";
            int statusChoice = view.getIntInput("Enter choice: ");
            
            TaskStatus status;
            switch (statusChoice) {
                case 1: status = TaskStatus::PENDING; break;
                case 2: status = TaskStatus::IN_PROGRESS; break;
                case 3: status = TaskStatus::COMPLETED; break;
                case 4: status = TaskStatus::FAILED; break;
                default: status = task->getStatus();
            }
            
            if (taskManager->updateTaskStatus(taskId, status)) {
                std::cout << "Task status updated successfully.\n";
                
                // Update task history
                auto it = taskHistories.find(taskId);
                if (it != taskHistories.end()) {
                    it->second->addState(status, "System", "Status changed to " + task->statusToString());
                }
                logger->info("Task status updated: ID " + std::to_string(taskId) + " to " + task->statusToString());
            } else {
                std::cout << "Failed to update task status.\n";
                logger->warning("Failed to update task status: ID " + std::to_string(taskId));
            }
        }
        catch (const std::exception& e) {
            logger->error("Error changing task status: " + std::string(e.what()));
            std::cout << "Failed to change task status: " << e.what() << std::endl;
        }
    }
    
    // Node Management Methods
    void addWorkerNode() {
        try {
            std::string name = view.getStringInput("Enter node name: ");
            int id = workerNodes.empty() ? 1 : workerNodes.back()->getId() + 1;
            
            auto newNode = std::make_shared<WorkerNode>(id, name);
            workerNodes.push_back(newNode);
            loadBalancer.addWorkerNode(newNode.get());
            
            // Register with monitor
            if (!monitors.empty()) {
                int monitorId = monitors[0]->getId();
                newNode->registerObserver(monitorId);
            }
            
            // Create and start worker thread for this node
            nodeThreads.emplace_back(std::thread([this, newNode]() {
                this->simulateTaskProcessing(newNode.get());
            }));
            
            std::cout << "Worker node added with ID: " << id << "\n";
            logger->info("Worker node added: " + name + " (ID: " + std::to_string(id) + ")");
        }
        catch (const std::exception& e) {
            logger->error("Error adding worker node: " + std::string(e.what()));
            std::cout << "Failed to add worker node: " << e.what() << std::endl;
        }
    }
    
    void viewAllNodes() {
        try {
            view.displayWorkerNodes(workerNodes);
        }
        catch (const std::exception& e) {
            logger->error("Error viewing nodes: " + std::string(e.what()));
            std::cout << "Failed to view nodes: " << e.what() << std::endl;
        }
    }
    
    void activateNode() {
        try {
            int nodeId = view.getIntInput("Enter node ID to activate: ");
            
            for (auto& node : workerNodes) {
                if (node->getId() == nodeId) {
                    node->activate();
                    std::cout << "Node activated.\n";
                    logger->info("Node activated: ID " + std::to_string(nodeId));
                    return;
                }
            }
            
            std::cout << "Node not found.\n";
            logger->warning("Failed to activate node: ID " + std::to_string(nodeId) + " - not found");
        }
        catch (const std::exception& e) {
            logger->error("Error activating node: " + std::string(e.what()));
            std::cout << "Failed to activate node: " << e.what() << std::endl;
        }
    }
    
    void deactivateNode() {
        try {
            int nodeId = view.getIntInput("Enter node ID to deactivate: ");
            
            for (auto& node : workerNodes) {
                if (node->getId() == nodeId) {
                    node->deactivate();
                    std::cout << "Node deactivated.\n";
                    logger->info("Node deactivated: ID " + std::to_string(nodeId));
                    return;
                }
            }
            
            std::cout << "Node not found.\n";
            logger->warning("Failed to deactivate node: ID " + std::to_string(nodeId) + " - not found");
        }
        catch (const std::exception& e) {
            logger->error("Error deactivating node: " + std::string(e.what()));
            std::cout << "Failed to deactivate node: " << e.what() << std::endl;
        }
    }
    
    void removeNode() {
        try {
            int nodeId = view.getIntInput("Enter node ID to remove: ");
            
            for (auto it = workerNodes.begin(); it != workerNodes.end(); ++it) {
                if ((*it)->getId() == nodeId) {
                    // Deactivate the node first to prevent processing new tasks
                    (*it)->deactivate();
                    
                    // Remove from load balancer
                    loadBalancer.removeWorkerNode(nodeId);
                    
                    // Erase from vector (will be deleted when shared_ptr reference count reaches 0)
                    workerNodes.erase(it);
                    
                    std::cout << "Node removed.\n";
                    logger->info("Node removed: ID " + std::to_string(nodeId));
                    return;
                }
            }
            
            std::cout << "Node not found.\n";
            logger->warning("Failed to remove node: ID " + std::to_string(nodeId) + " - not found");
        }
        catch (const std::exception& e) {
            logger->error("Error removing node: " + std::string(e.what()));
            std::cout << "Failed to remove node: " << e.what() << std::endl;
        }
    }
    
    // Monitoring Methods
    void viewNodeStatus() {
        try {
            if (workerNodes.empty()) {
                std::cout << "No worker nodes found.\n";
                return;
            }
            
            std::cout << "\n===== Node Status =====\n";
            for (const auto& node : workerNodes) {
                std::cout << "Node ID: " << node->getId() << "\n";
                std::cout << "Name: " << node->getName() << "\n";
                std::cout << "Status: " << (node->isActive() ? "Active" : "Inactive") << "\n";
                std::cout << "Tasks in queue: " << node->getQueueSize() << "\n";
                std::cout << "------------------------\n";
            }
        }
        catch (const std::exception& e) {
            logger->error("Error viewing node status: " + std::string(e.what()));
            std::cout << "Failed to view node status: " << e.what() << std::endl;
        }
    }
    
    void viewTaskQueueSizes() {
        try {
            if (workerNodes.empty()) {
                std::cout << "No worker nodes found.\n";
                return;
            }
            
            std::cout << "\n===== Task Queue Sizes =====\n";
            for (const auto& node : workerNodes) {
                std::cout << node->getName() << ": " << node->getQueueSize() << " tasks\n";
            }
        }
        catch (const std::exception& e) {
            logger->error("Error viewing task queue sizes: " + std::string(e.what()));
            std::cout << "Failed to view task queue sizes: " << e.what() << std::endl;
        }
    }
    
    void viewTaskHistories() {
        try {
            if (taskHistories.empty()) {
                std::cout << "No task histories found.\n";
                return;
            }
            
            int taskId = view.getIntInput("Enter task ID to view history (0 for all): ");
            
            std::lock_guard<std::mutex> lock(taskMutex);
            if (taskId == 0) {
                for (const auto& pair : taskHistories) {
                    displayTaskHistory(pair.first, pair.second);
                }
            } else {
                auto historyIt = taskHistories.find(taskId);
                if (historyIt != taskHistories.end()) {
                    displayTaskHistory(taskId, historyIt->second);
                } else {
                    std::cout << "Task history not found.\n";
                    logger->warning("Task history not found: ID " + std::to_string(taskId));
                }
            }
        }
        catch (const std::exception& e) {
            logger->error("Error viewing task histories: " + std::string(e.what()));
            std::cout << "Failed to view task histories: " << e.what() << std::endl;
        }
    }
    
    void displayTaskHistory(int taskId, const std::shared_ptr<TaskHistory>& history) {
        try {
            Task* taskPtr = taskManager->getTaskById(taskId);
            std::shared_ptr<Task> task(taskPtr, [](Task*) {});
            if (!task) {
                std::cout << "Task #" << taskId << " not found (history exists)\n";
                return;
            }
            
            std::cout << "\n===== History for Task #" << taskId << ": " << task->getTitle() << " =====\n";
            
            auto states = history->getHistory();
            for (const auto& state : states) {
                std::string statusStr;
                switch (state.getStatus()) {
                    case TaskStatus::PENDING: statusStr = "Pending"; break;
                    case TaskStatus::IN_PROGRESS: statusStr = "In Progress"; break;
                    case TaskStatus::COMPLETED: statusStr = "Completed"; break;
                    case TaskStatus::FAILED: statusStr = "Failed"; break;
                    default: statusStr = "Unknown";
                }
                
                time_t timestamp = state.getTimestamp();
                std::cout << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S");
                std::cout << " - Status: " << statusStr;
                std::cout << " - By: " << state.getChangedBy();
                std::cout << " - Comment: " << state.getComments() << "\n";
            }
        }
        catch (const std::exception& e) {
            logger->error("Error displaying task history: " + std::string(e.what()));
            std::cout << "Failed to display task history: " << e.what() << std::endl;
        }
    }
    
    // Task Distribution Methods
    void changeLoadBalancingStrategy() {
        try {
            std::cout << "Select load balancing strategy:\n";
            std::cout << "1. Round Robin\n";
            std::cout << "2. Least Loaded\n";
            std::cout << "3. Priority Based\n";
            int strategyChoice = view.getIntInput("Enter choice: ");
            
            std::shared_ptr<LoadBalancingStrategy> newStrategy;
            std::string strategyName;
            
            switch (strategyChoice) {
                case 1:
                    newStrategy = std::make_shared<RoundRobinStrategy>();
                    strategyName = "Round Robin";
                    break;
                case 2:
                    newStrategy = std::make_shared<LeastLoadedStrategy>();
                    strategyName = "Least Loaded";
                    break;
                case 3:
                    newStrategy = std::make_shared<PriorityBasedStrategy>();
                    strategyName = "Priority Based";
                    break;
                default:
                    std::cout << "Invalid choice. Strategy not changed.\n";
                    return;
            }
            
            loadBalancer.setStrategy(newStrategy.get());
            std::cout << "Strategy changed to " << strategyName << ".\n";
            logger->info("Load balancing strategy changed to: " + strategyName);
        }
        catch (const std::exception& e) {
            logger->error("Error changing load balancing strategy: " + std::string(e.what()));
            std::cout << "Failed to change load balancing strategy: " << e.what() << std::endl;
        }
    }
    
    void distributePendingTasks() {
        try {
            std::lock_guard<std::mutex> lock(taskMutex);
            auto pendingTasks = taskManager->getTasksByStatus(TaskStatus::PENDING);
            
            if (pendingTasks.empty()) {
                std::cout << "No pending tasks to distribute.\n";
                return;
            }
            
            if (workerNodes.empty()) {
                std::cout << "No worker nodes available for distribution.\n";
                return;
            }
            
            int distributedCount = 0;
            for (const auto& task : pendingTasks) {
                if (loadBalancer.distributeTask(task)) {
                    taskManager->updateTaskStatus(task.getId(), TaskStatus::IN_PROGRESS);
                    
                    // Update task history
                    auto historyIt = taskHistories.find(task.getId());
                    if (historyIt != taskHistories.end()) {
                        historyIt->second->addState(
                            TaskStatus::IN_PROGRESS, "System", "Task distributed to worker node");
                    }
                    
                    distributedCount++;
                }
            }
            
            std::cout << "Distributed " << distributedCount << " out of " << pendingTasks.size() << " tasks.\n";
            logger->info("Distributed " + std::to_string(distributedCount) + " tasks");
        }
        catch (const std::exception& e) {
            logger->error("Error distributing tasks: " + std::string(e.what()));
            std::cout << "Failed to distribute tasks: " << e.what() << std::endl;
        }
    }
    
    void viewTaskDistribution() {
        try {
            if (workerNodes.empty()) {
                std::cout << "No worker nodes available.\n";
                return;
            }
            
            std::cout << "\n===== Task Distribution =====\n";
            for (const auto& node : workerNodes) {
                std::cout << "Node: " << node->getName() << " (ID: " << node->getId() << ")\n";
                std::cout << "Active: " << (node->isActive() ? "Yes" : "No") << "\n";
                std::cout << "Tasks in queue: " << node->getQueueSize() << "\n";
                
                std::vector<int> taskIds;
                if (!taskIds.empty()) {
                    std::cout << "Task IDs: ";
                    for (size_t i = 0; i < taskIds.size(); ++i) {
                        std::cout << taskIds[i];
                        if (i < taskIds.size() - 1) {
                            std::cout << ", ";
                        }
                    }
                    std::cout << "\n";
                }
                std::cout << "------------------------\n";
            }
        }
        catch (const std::exception& e) {
            logger->error("Error viewing task distribution: " + std::string(e.what()));
            std::cout << "Failed to view task distribution: " << e.what() << std::endl;
        }
    }
    
    // Worker node thread simulation
    void simulateTaskProcessing(WorkerNode* node) {
        logger->debug("Task processing thread started for node " + node->getName());
        
        while (running) {
            if (node->isActive() && node->getQueueSize() > 0) {
                // Get task from node's queue
                std::shared_ptr<Task> currentTask = node->getNextTask();
                
                if (currentTask) {
                    int taskId = currentTask->getId();
                    
                    // Simulate processing time based on priority
                    int processingTime = 0;
                    switch (currentTask->getPriority()) {
                        case TaskPriority::LOW: processingTime = 5; break;
                        case TaskPriority::MEDIUM: processingTime = 3; break;
                        case TaskPriority::HIGH: processingTime = 2; break;
                        case TaskPriority::CRITICAL: processingTime = 1; break;
                    }
                    
                    // Log task processing started
                    logger->debug("Node " + node->getName() + " processing task #" + 
                                 std::to_string(taskId) + " for " + 
                                 std::to_string(processingTime) + " seconds");
                    
                    // Simulate work
                    std::this_thread::sleep_for(std::chrono::seconds(processingTime));
                    
                    // Task completed
                    {
                        std::lock_guard<std::mutex> lock(taskMutex);
                        taskManager->updateTaskStatus(taskId, TaskStatus::COMPLETED);
                        
                        // Update task history
                        auto historyIt = taskHistories.find(taskId);
                        if (historyIt != taskHistories.end()) {
                            historyIt->second->addState(
                                TaskStatus::COMPLETED, 
                                node->getName(), 
                                "Task completed by worker node");
                        }
                    }
                    
                    logger->info("Node " + node->getName() + " completed task #" + std::to_string(taskId));
                }
            } else {
                // If inactive or no tasks, sleep for a bit
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        
        logger->debug("Task processing thread stopped for node " + node->getName());
    }
};

int main() {
    try {
        TaskManagementController controller;
        controller.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        LoggingService::getInstance()->error("Fatal error in main: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}