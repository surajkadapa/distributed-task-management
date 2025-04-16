#include "../strategies/TaskPriorityStrategy.h"
#include <algorithm>

// Sort tasks by due date (earliest first)
std::vector<std::shared_ptr<Task>> DueDatePriorityStrategy::prioritizeTasks(
    const std::vector<std::shared_ptr<Task>>& tasks) {
    
    auto sortedTasks = tasks;
    std::sort(sortedTasks.begin(), sortedTasks.end(), 
              [](const std::shared_ptr<Task>& a, const std::shared_ptr<Task>& b) {
                  return a->getDueDate() < b->getDueDate();
              });
    
    return sortedTasks;
}

// Sort tasks by priority level (highest first)
std::vector<std::shared_ptr<Task>> ImportancePriorityStrategy::prioritizeTasks(
    const std::vector<std::shared_ptr<Task>>& tasks) {
    
    auto sortedTasks = tasks;
    std::sort(sortedTasks.begin(), sortedTasks.end(), 
              [](const std::shared_ptr<Task>& a, const std::shared_ptr<Task>& b) {
                  return static_cast<int>(a->getPriority()) > static_cast<int>(b->getPriority());
              });
    
    return sortedTasks;
}

// Sort tasks by a combination of priority and due date
std::vector<std::shared_ptr<Task>> HybridPriorityStrategy::prioritizeTasks(
    const std::vector<std::shared_ptr<Task>>& tasks) {
    
    auto sortedTasks = tasks;
    std::sort(sortedTasks.begin(), sortedTasks.end(), 
              [](const std::shared_ptr<Task>& a, const std::shared_ptr<Task>& b) {
                  // First compare by priority
                  if (a->getPriority() != b->getPriority()) {
                      return static_cast<int>(a->getPriority()) > static_cast<int>(b->getPriority());
                  }
                  
                  // If priorities are equal, compare by due date
                  return a->getDueDate() < b->getDueDate();
              });
    
    return sortedTasks;
}