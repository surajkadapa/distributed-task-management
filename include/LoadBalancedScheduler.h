#pragma once
#include "Scheduler.h"

class LoadBalancedScheduler : public Scheduler {
public:
    int pickNode(const std::vector<std::shared_ptr<Node>>& nodes) override;
};