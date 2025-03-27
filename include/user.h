#ifndef USER_H
#define USER_H

#include <string>
#include "Task.h"

class User {
public:
    std::string userID;
    std::string role;

    void submitTask(Task& task);
    void viewTaskStatus(Task& task);
    void cancelTask(Task& task);
};

#endif
