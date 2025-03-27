#ifndef TASK_H
#define TASK_H

#include <string>
#include <map>

class Task {
public:
    Task(std::string id, int priority);
    void execute();
    std::string getStatus() const;
    std::map<std::string, std::string> getResult() const;

private:
    std::string taskID;
    int priority;
    std::string status;
    std::map<std::string, std::string> data;
};

#endif
