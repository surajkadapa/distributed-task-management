#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "Task.h"
#include "WorkerNode.h"
#include <vector>

class TaskManager {
public:
    void scheduleTask(Task& task);
    void distributeTask(Task& task);
    void monitorTasks();
    void retryFailedTasks();
    void logTaskActivity();
    void getTaskHistory();

private:
    std::vector<Task> tasks;
    std::vector<WorkerNode> workerNodes;
};

#endif
