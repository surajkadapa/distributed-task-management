#include "../include/Node.h"
#include "../include/TaskManager.h"  // This is needed for Node.cpp to access TaskManager methods
#include "../include/Scheduler.h"
#include <chrono>
#include <iostream>
#include <algorithm>

Node::Node(int id) : id(id), busy(false), running(false), taskCount(0) {}

Node::Node(int id, TaskManager* manager) 
    : id(id), busy(false), running(false), taskCount(0), taskManager(manager) {}


void Node::start() {
    running = true;
    worker = std::thread(&Node::processTasks, this);
}

void Node::stop() {
    running = false;
    cv.notify_all();
    if (worker.joinable()) worker.join();
}

void Node::addTask(std::shared_ptr<Task> task) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        taskQueue.push(task);
        taskIDs.push_back(task->getId());  // Track task ID
        taskCount++;
        std::cout << "Task ID: " << task->getId() << " added to Node " << id << std::endl;
    }
    cv.notify_one();
}

bool Node::isBusy() const {
    return busy.load();
}

int Node::getId() const {
    return id;
}

int Node::getTaskCount() const {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Node " << id << " task count: " << taskCount << std::endl;
    return taskCount;
}

std::vector<int> Node::getTaskIDs() const {
    std::lock_guard<std::mutex> lock(mtx);
    return taskIDs;
}

std::vector<std::shared_ptr<Task>> Node::getTaskQueueSnapshot() {
    std::lock_guard<std::mutex> lock(mtx);
    std::queue<std::shared_ptr<Task>> copy = taskQueue;
    std::vector<std::shared_ptr<Task>> tasks;
    while (!copy.empty()) {
        tasks.push_back(copy.front());
        copy.pop();
    }
    return tasks;
}

void Node::processTasks() {
    while (running) {
        std::shared_ptr<Task> task;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] { return !taskQueue.empty() || !running; });

            if (!running && taskQueue.empty())
                break;

            if (!taskQueue.empty()) {
                task = taskQueue.front();
                taskQueue.pop();
                busy = true;
                task->setStatus(TaskStatus::Running);  // Update status to Running
                std::cout << "Processing Task ID: " << task->getId() << std::endl;
            }
        }

        if (task) {
            std::this_thread::sleep_for(std::chrono::seconds(task->getDuration()));
            task->setStatus(TaskStatus::Completed);  // Update status to Completed
            std::cout << "Task ID: " << task->getId() << " Completed." << std::endl;

            {
                std::lock_guard<std::mutex> lock(mtx);
                if (taskCount > 0) {
                    taskCount--;
                    // Remove from taskIDs
                    taskIDs.erase(std::remove(taskIDs.begin(), taskIDs.end(), task->getId()), taskIDs.end());
                    std::cout << "Node " << id << " task count decremented. Current count: " << taskCount << std::endl;
                }
            }
        }

        busy = false;

        // Check for pending tasks in the TaskManager and assign if possible
        if (running && taskManager) { //check if the node is still running and taskManager is valid
            std::lock_guard<std::mutex> managerLock(taskManager->mtx); //added lock
            for (auto& pendingTask : taskManager->tasks) {
                if (pendingTask->getStatus() == TaskStatus::Pending) {
                    int nodeIndex = taskManager->scheduler->pickNode(taskManager->nodes);
                    if (nodeIndex == id-1) { //make sure the task is assigned to the current node.
                        addTask(pendingTask);
                        std::cout << "Reassigned pending task '" << pendingTask->getName()
                                  << "' to Node " << id << std::endl;
                        break; // Assign only one task at a time
                    }
                }
            }
        }
    }
}