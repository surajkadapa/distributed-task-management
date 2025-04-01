// include/loadbalancer.h
#ifndef LOAD_BALANCER_H
#define LOAD_BALANCER_H

#include "workernode.h"
#include "task.h"
#include <vector>
#include <memory>

// Strategy Pattern
class LoadBalancingStrategy {
public:
    virtual ~LoadBalancingStrategy() = default;
    virtual int selectWorkerNode(const std::vector<WorkerNode*>& nodes, const Task& task) = 0;
};

class RoundRobinStrategy : public LoadBalancingStrategy {
private:
    int lastSelectedIndex;
    
public:
    RoundRobinStrategy();
    int selectWorkerNode(const std::vector<WorkerNode*>& nodes, const Task& task) override;
};

class LeastLoadedStrategy : public LoadBalancingStrategy {
public:
    int selectWorkerNode(const std::vector<WorkerNode*>& nodes, const Task& task) override;
};

class PriorityBasedStrategy : public LoadBalancingStrategy {
public:
    int selectWorkerNode(const std::vector<WorkerNode*>& nodes, const Task& task) override;
};

// Facade Pattern
class LoadBalancer {
private:
    std::vector<WorkerNode*> workerNodes;
    LoadBalancingStrategy* strategy;
    
public:
    LoadBalancer();
    ~LoadBalancer();
    
    // Node management
    void addWorkerNode(WorkerNode* node);
    void removeWorkerNode(int nodeId);
    
    // Strategy setter (Strategy Pattern)
    void setStrategy(LoadBalancingStrategy* newStrategy);
    
    // Task distribution
    bool distributeTask(const Task& task);
    
    // Getters
    std::vector<WorkerNode*> getWorkerNodes() const;
};

#endif // LOAD_BALANCER_H