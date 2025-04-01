#ifndef TASK_H
#define TASK_H

#include <string>
#include <vector>
#include <ctime>

enum class TaskStatus { PENDING, IN_PROGRESS, COMPLETED, FAILED };
enum class TaskPriority { LOW, MEDIUM, HIGH, CRITICAL };

class Task {
private:
    int taskId;
    std::string title;
    std::string description;
    TaskPriority priority;
    TaskStatus status;
    time_t creationTime;
    time_t dueTime;
    int assignedUserId;
    int createdByUserId;

public:
    Task();
    Task(int id, const std::string& title, const std::string& desc, 
         TaskPriority priority, int assignedUser, int createdBy);
    
    // Getters
    int getId() const;
    std::string getTitle() const;
    std::string getDescription() const;
    TaskPriority getPriority() const;
    TaskStatus getStatus() const;
    time_t getCreationTime() const;
    time_t getDueTime() const;
    int getAssignedUserId() const;
    int getCreatedByUserId() const;
    
    // Setters
    void setTitle(const std::string& title);
    void setDescription(const std::string& desc);
    void setPriority(TaskPriority priority);
    void setStatus(TaskStatus status);
    void setDueTime(time_t dueTime);
    void setAssignedUserId(int userId);
    
    // Utility methods
    std::string priorityToString() const;
    std::string statusToString() const;
};

#endif // TASK_H