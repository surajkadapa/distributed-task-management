// include/scheduler.h
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"
#include <vector>
#include <queue>
#include <functional>

// Command Pattern
class SchedulerCommand {
public:
    virtual ~SchedulerCommand() = default;
    virtual void execute() = 0;
};

class ExecuteTaskCommand : public SchedulerCommand {
private:
    Task task;
    std::function<void(const Task&)> executor;
    
public:
    ExecuteTaskCommand(const Task& task, std::function<void(const Task&)> executor);
    void execute() override;
};

class Scheduler {
private:
    std::priority_queue<std::pair<time_t, SchedulerCommand*>> scheduledTasks;
    bool running;
    
public:
    Scheduler();
    ~Scheduler();
    
    // Scheduling operations
    void scheduleTask(time_t executionTime, SchedulerCommand* command);
    void processScheduledTasks();
    
    // Control methods
    void start();
    void stop();
    bool isRunning() const;
    
    // Utility
    int getTaskCount() const;
    void clearAllTasks();
};

#endif // SCHEDULER_H