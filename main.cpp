#include "/Users/gaganphadke/OOAD/Project/distributed-task-management/src/taskmanager.h"
#include "/Users/gaganphadke/OOAD/Project/distributed-task-management/src/workernode.h"
#include "/Users/gaganphadke/OOAD/Project/distributed-task-management/src/task.h"
#include <iostream>

int main() {
    TaskManager manager;
    WorkerNode node("Node1", 4);
    Task task1("Task1", 1);

    manager.scheduleTask(task1);
    manager.distributeTask(task1);
    manager.monitorTasks();

    return 0;
}
