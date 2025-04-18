#include "../include/RoundRobinScheduler.h"
#include "../include/Node.h"

RoundRobinScheduler::RoundRobinScheduler() : lastAssignedIndex(0) {}

int RoundRobinScheduler::pickNode(const std::vector<std::shared_ptr<Node>>& nodes) {
    if (nodes.empty()) {
        return -1;
    }
    
    // Start from the next index after the last assigned node
    size_t startIndex = lastAssignedIndex;
    size_t currentIndex = (startIndex + 1) % nodes.size();
    
    // Try to find a non-busy node, starting from the next one
    while (currentIndex != startIndex) {
        if (!nodes[currentIndex]->isBusy()) {
            lastAssignedIndex = currentIndex;
            return currentIndex;
        }
        currentIndex = (currentIndex + 1) % nodes.size();
    }
    
    // Check the start index as well
    if (!nodes[startIndex]->isBusy()) {
        return startIndex;
    }
    
    return -1; // All nodes busy
}