#ifndef TASK_H
#define TASK_H

#include <string>
#include <map>

class Task {
private:
    std::string taskID;
    int priority;
    std::string status;
    std::map<std::string, std::string> data;

public:
    // Constructor
    Task(std::string id, int priority);

    // Method to execute the task
    void execute();

    // Getter for task status
    std::string getStatus() const;

    // Getter for task result
    std::map<std::string, std::string> getResult() const;

    // Getter for priority
    int getPriority() const { return priority; }
};

#endif // TASK_H
