#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include <vector>
#include <memory>
#include "Task.h"
#include "User.h"

class Project {
private:
    int id;
    std::string name;
    std::string description;
    std::string startDate;
    std::string endDate;
    std::vector<std::shared_ptr<Task>> tasks;
    std::vector<std::shared_ptr<User>> members;
    std::shared_ptr<User> owner;

public:
    Project(int id, const std::string& name, const std::string& description,
            const std::string& startDate, const std::string& endDate, 
            std::shared_ptr<User> owner);
    
    int getId() const;
    std::string getName() const;
    std::string getDescription() const;
    std::string getStartDate() const;
    std::string getEndDate() const;
    std::shared_ptr<User> getOwner() const;
    
    // Project management
    void addTask(std::shared_ptr<Task> task);
    void removeTask(int taskId);
    std::vector<std::shared_ptr<Task>> getTasks() const;
    
    void addMember(std::shared_ptr<User> user);
    void removeMember(int userId);
    std::vector<std::shared_ptr<User>> getMembers() const;
    
    // JSON serialization
    std::string toJson() const;
};

#endif // PROJECT_H