#ifndef TASK_FACTORY_H
#define TASK_FACTORY_H

#include <memory>
#include <string>
#include "Task.h"

class TaskFactory {
public:
    static std::shared_ptr<Task> createTask(const std::string& type, 
                                          int id, 
                                          const std::string& title, 
                                          const std::string& description,
                                          TaskStatus status,
                                          TaskPriority priority,
                                          const std::string& createdDate,
                                          const std::string& dueDate,
                                          std::shared_ptr<User> creator) {
        
        if (type == "feature") {
            return std::make_shared<FeatureTask>(id, title, description, status, 
                                                priority, createdDate, dueDate, creator);
        } 
        else if (type == "bug") {
            return std::make_shared<BugTask>(id, title, description, status, 
                                           priority, createdDate, dueDate, creator);
        }
        // Default to a standard task if type is not recognized
        return std::make_shared<FeatureTask>(id, title, description, status, 
                                           priority, createdDate, dueDate, creator);
    }
};

#endif // TASK_FACTORY_H