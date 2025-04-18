// LoadBalancedScheduler.cpp
#include "../include/LoadBalancedScheduler.h"
#include "../include/Node.h"
#include <algorithm>

int LoadBalancedScheduler::pickNode(const std::vector<std::shared_ptr<Node>>& nodes) {
    if (nodes.empty()) {
        return -1;
    }
    
    int minTasksIndex = -1;
    int minTasks = std::numeric_limits<int>::max();
    
    // Find the node with the fewest tasks
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (!nodes[i]->isBusy()) {
            int taskCount = nodes[i]->getTaskCount();
            if (taskCount < minTasks) {
                minTasks = taskCount;
                minTasksIndex = i;
            }
        }
    }
    
    return minTasksIndex;
}