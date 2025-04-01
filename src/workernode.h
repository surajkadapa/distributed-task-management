#ifndef WORKERNODE_H
#define WORKERNODE_H

#include <string>
#include "task.h"

class WorkerNode {
private:
    std::string nodeID;
    int availableResources;

public:
    // Constructor
    WorkerNode(std::string id, int resources);

    // Execute a given task
    void executeTask(Task& task);

    // Report the status of the worker node
    void reportStatus();

    // Send a heartbeat signal
    void heartbeat();

    // Recover and re-execute a failed task
    void recoverTask(Task& task);
    
};

#endif // WORKERNODE_H
