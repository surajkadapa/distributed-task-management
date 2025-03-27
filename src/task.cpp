#include "task.h"

Task::Task(std::string id, int priority) : taskID(id), priority(priority), status("Pending") {}

void Task::execute() {
    // Execution logic here
    status = "Completed";
}

std::string Task::getStatus() const {
    return status;
}

std::map<std::string, std::string> Task::getResult() const {
    return data;
}
