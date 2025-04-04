#include "../include/FIFOScheduler.h"

int FIFOScheduler::pickNode(const std::vector<std::shared_ptr<Node>>& nodes) {
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (!nodes[i]->isBusy()) {
            return i;
        }
    }
    return -1; // All nodes busy or none available
}
