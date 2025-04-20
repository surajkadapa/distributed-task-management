#pragma once
#include <vector>
#include <memory>

// Forward declarations to break circular dependencies
class Node;

class Scheduler {
public:
    virtual int pickNode(const std::vector<std::shared_ptr<Node>>& nodes) = 0;
    virtual ~Scheduler() = default;
};