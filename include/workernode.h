#ifndef WORKERNODE_H
#define WORKERNODE_H

#include "Task.h"
#include <string>

class WorkerNode {
public:
    WorkerNode(std::string id, int resources);
    void executeTask(Task& task);
    void reportStatus();
    void heartbeat();
    void recoverTask(Task& task);

private:
    std::string nodeID;
    int availableResources;
};

#endif
