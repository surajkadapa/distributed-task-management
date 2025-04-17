#pragma once
#include "Task.h"
#include "Node.h"
#include <vector>
#include <memory>
class Node;
class Scheduler {
public:
    virtual int pickNode(const std::vector<std::shared_ptr<Node>>& nodes) = 0;
    virtual ~Scheduler() = default;
};
