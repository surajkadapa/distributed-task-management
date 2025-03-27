#include "loadbalancer.h"

void LoadBalancer::distributeRequests(std::vector<WorkerNode>& nodes, Task& task) {
    if (nodes.empty()) return;
    nodes[0].executeTask(task);
}
