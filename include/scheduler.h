#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "WorkerNode.h"
#include "Task.h"
#include <vector>

class Scheduler {
public:
    WorkerNode selectWorkerNode(const std::vector<WorkerNode>& nodes);
};

#endif
