#ifndef TASK_H
#define TASK_H

#include <string>
#include <vector>
#include <memory>
#include "User.h"
#include "TaskComment.h"
#include "TaskAttachment.h"

enum class TaskStatus {
    TODO,
    IN_PROGRESS,
    REVIEW,
    COMPLETED,
    DEFERRED
};

enum class TaskPriority {
    LOW,
    MEDIUM,
    HIGH,
    URGENT
};

// Abstract Task class
class Task {
protected:
    int id;
    std::string title;
    std::string description;
    TaskStatus status;
    TaskPriority priority;
    std::string createdDate;
    std::string dueDate;
    std::shared_ptr<User> creator;
    std::shared_ptr<User> assignee;
    std::vector<TaskComment> comments;
    std::vector<TaskAttachment> attachments;
    std::vector<std::string> history;

public:
    Task(int id, const std::string& title, const std::string& description,
         TaskStatus status, TaskPriority priority,
         const std::string& createdDate, const std::string& dueDate,
         std::shared_ptr<User> creator);
    
    virtual ~Task() = default;
    
    // Getters
    int getId() const;
    std::string getTitle() const;
    std::string getDescription() const;
    TaskStatus getStatus() const;
    TaskPriority getPriority() const;
    std::string getCreatedDate() const;
    std::string getDueDate() const;
    std::shared_ptr<User> getCreator() const;
    std::shared_ptr<User> getAssignee() const;
    
    // Setters
    void setTitle(const std::string& title);
    void setDescription(const std::string& description);
    void setStatus(TaskStatus status);
    void setPriority(TaskPriority priority);
    void setDueDate(const std::string& dueDate);
    void assignTo(std::shared_ptr<User> user);
    
    // Comments and attachments
    void addComment(const TaskComment& comment);
    void addAttachment(const TaskAttachment& attachment);
    std::vector<TaskComment> getComments() const;
    std::vector<TaskAttachment> getAttachments() const;
    
    // History tracking
    void addHistoryEntry(const std::string& entry);
    std::vector<std::string> getHistory() const;
    
    // Factory method pattern - create different types of tasks
    static std::shared_ptr<Task> createTask(const std::string& type, 
                                          int id, const std::string& title, 
                                          const std::string& description,
                                          TaskPriority priority,
                                          const std::string& dueDate,
                                          std::shared_ptr<User> creator);
    
    // Each subclass must implement completion behavior
    virtual bool complete() = 0;
    
    // JSON serialization
    virtual std::string toJson() const;
};

// Feature Task concrete class
class FeatureTask : public Task {
private:
    std::string featureSpecification;
    
public:
    FeatureTask(int id, const std::string& title, const std::string& description,
               TaskStatus status, TaskPriority priority,
               const std::string& createdDate, const std::string& dueDate,
               std::shared_ptr<User> creator);
    
    void setFeatureSpecification(const std::string& spec);
    std::string getFeatureSpecification() const;
    
    // Implementation of completion behavior
    bool complete() override;
    
    // JSON serialization
    std::string toJson() const override;
};

// Bug Task concrete class
class BugTask : public Task {
private:
    std::string stepsToReproduce;
    std::string expectedBehavior;
    std::string actualBehavior;
    
public:
    BugTask(int id, const std::string& title, const std::string& description,
           TaskStatus status, TaskPriority priority,
           const std::string& createdDate, const std::string& dueDate,
           std::shared_ptr<User> creator);
    
    void setReproductionSteps(const std::string& steps);
    void setExpectedBehavior(const std::string& expected);
    void setActualBehavior(const std::string& actual);
    
    std::string getReproductionSteps() const;
    std::string getExpectedBehavior() const;
    std::string getActualBehavior() const;
    
    // Implementation of completion behavior
    bool complete() override;
    
    // JSON serialization
    std::string toJson() const override;
};

#endif // TASK_H