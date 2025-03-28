#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <vector>
#include "task.h"
#include "workernode.h"

class TaskManager {
private:
    std::vector<Task> tasks;
    std::vector<WorkerNode> workerNodes;

public:
    // Schedule a task
    void scheduleTask(Task& task);

    // Distribute a task to a worker node
    void distributeTask(Task& task);

    // Monitor all tasks
    void monitorTasks();

    // Retry any failed tasks
    void retryFailedTasks();

    // Log task activity
    void logTaskActivity();

    // Get task history
    void getTaskHistory();
};

#endif // TASKMANAGER_H
