#include "scheduler.h"

WorkerNode Scheduler::selectWorkerNode(const std::vector<WorkerNode>& nodes) {
    // Simple round-robin selection
    static int index = 0;
    if (nodes.empty()) throw std::runtime_error("No available worker nodes!");
    index = (index + 1) % nodes.size();
    return nodes[index];
}
