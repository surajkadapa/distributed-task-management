#include "user.h"
#include <iostream>

void User::submitTask(Task& task) {
    std::cout << "User " << userID << " submitted task: " << task.getStatus() << std::endl;
}

void User::viewTaskStatus(Task& task) {
    std::cout << "Task Status: " << task.getStatus() << std::endl;
}

void User::cancelTask(Task& task) {
    task.status = "Cancelled";
    std::cout << "Task Cancelled." << std::endl;
}
