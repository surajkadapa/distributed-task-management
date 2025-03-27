#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include "WorkerNode.h"
#include <vector>

class LoadBalancer {
public:
    void distributeRequests(std::vector<WorkerNode>& nodes, Task& task);
};

#endif
