#ifndef TASK_PRIORITY_STRATEGY_H
#define TASK_PRIORITY_STRATEGY_H

#include <vector>
#include <memory>
#include "../models/Task.h"

class TaskPriorityStrategy {
public:
    virtual ~TaskPriorityStrategy() = default;
    virtual std::vector<std::shared_ptr<Task>> prioritizeTasks(
        const std::vector<std::shared_ptr<Task>>& tasks) = 0;
};

// Prioritize by due date
class DueDatePriorityStrategy : public TaskPriorityStrategy {
public:
    std::vector<std::shared_ptr<Task>> prioritizeTasks(
        const std::vector<std::shared_ptr<Task>>& tasks) override;
};

// Prioritize by task priority field
class ImportancePriorityStrategy : public TaskPriorityStrategy {
public:
    std::vector<std::shared_ptr<Task>> prioritizeTasks(
        const std::vector<std::shared_ptr<Task>>& tasks) override;
};

// Combined prioritization strategy
class HybridPriorityStrategy : public TaskPriorityStrategy {
public:
    std::vector<std::shared_ptr<Task>> prioritizeTasks(
        const std::vector<std::shared_ptr<Task>>& tasks) override;
};

#endif // TASK_PRIORITY_STRATEGY_H