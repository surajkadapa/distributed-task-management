#include "taskhistory.h"
#include <iostream>

void TaskHistory::storeTaskResult(Task& task) {
    std::cout << "Storing task result: " << task.getStatus() << std::endl;
}
