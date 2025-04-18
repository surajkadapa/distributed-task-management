#pragma once
#include "Scheduler.h"

class RoundRobinScheduler : public Scheduler {
private:
    size_t lastAssignedIndex;

public:
    RoundRobinScheduler();
    int pickNode(const std::vector<std::shared_ptr<Node>>& nodes) override;
};