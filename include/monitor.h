#ifndef MONITOR_H
#define MONITOR_H

#include "Task.h"
#include <vector>

class Monitor {
public:
    void trackTaskStatus(const Task& task);
    void generateReport();
};

#endif
