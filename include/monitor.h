// include/monitor.h
#ifndef MONITOR_H
#define MONITOR_H

#include "workernode.h"
#include <vector>
#include <string>

// Observer Pattern - Observer
class Monitor {
private:
    int monitorId;
    std::string name;
    std::vector<std::string> logs;
    
public:
    Monitor(int id, const std::string& name);
    
    // Observer method
    void update(const std::string& message);
    
    // Monitoring operations
    void checkNodeStatus(const WorkerNode& node);
    std::vector<std::string> getLogs() const;
    void clearLogs();
    
    // Getters
    int getId() const;
    std::string getName() const;
};

#endif // MONITOR_H