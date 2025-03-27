#include "monitor.h"
#include <iostream>

void Monitor::trackTaskStatus(const Task& task) {
    std::cout << "Tracking task: " << task.getStatus() << std::endl;
}

void Monitor::generateReport() {
    std::cout << "Generating system report..." << std::endl;
}
