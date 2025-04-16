#include "../observers/NotificationObserver.h"
#include "../models/User.h"
#include "../models/Task.h"
#include "../controllers/NotificationController.h"
#include <iostream>
#include <memory>
#include "../controllers/UserController.h"
#include "../controllers/TaskController.h"


void EmailNotificationObserver::update(const std::string& event, std::shared_ptr<void> data) {
    if (event == "task_assigned") {
        auto taskData = std::static_pointer_cast<std::pair<int, int>>(data);
        int taskId = taskData->first;
        int userId = taskData->second;
        
        // Get user email
        auto user = UserController::getUserById(userId);
        if (!user) return;
        
        // Get task details
        auto task = TaskController::getTaskById(taskId);
        if (!task) return;
        
        // Send email notification
        std::string subject = "Task Assigned: " + task->getTitle();
        std::string body = "You have been assigned a new task: " + task->getTitle() + 
                           "\nDue Date: " + task->getDueDate();
        
        sendEmail(user->getEmail(), subject, body);
    }
    else if (event == "task_completed") {
        // Handle task completion notifications
        int taskId = *static_cast<int*>(data.get());
        
        // Get task details
        auto task = TaskController::getTaskById(taskId);
        if (!task) return;
        
        // Get creator email
        auto creator = task->getCreator();
        if (!creator) return;
        
        // Send email notification
        std::string subject = "Task Completed: " + task->getTitle();
        std::string body = "A task you created has been completed: " + task->getTitle();
        
        sendEmail(creator->getEmail(), subject, body);
    }
    // Other event types would be handled similarly
}

void EmailNotificationObserver::sendEmail(const std::string& recipient, 
                                         const std::string& subject, 
                                         const std::string& body) {
    // In a real implementation, this would connect to an email service
    // For this example, we'll just print to console
    std::cout << "Sending email to: " << recipient << std::endl;
    std::cout << "Subject: " << subject << std::endl;
    std::cout << "Body: " << body << std::endl;
}

void InAppNotificationObserver::update(const std::string& event, std::shared_ptr<void> data) {
    NotificationType type;
    int userId = 0;
    int entityId = 0;
    std::string message;
    
    if (event == "task_assigned") {
        auto taskData = std::static_pointer_cast<std::pair<int, int>>(data);
        entityId = taskData->first;  // Task ID
        userId = taskData->second;   // User ID
        
        auto task = TaskController::getTaskById(entityId);
        if (!task) return;
        
        type = NotificationType::TASK_ASSIGNED;
        message = "You have been assigned a new task: " + task->getTitle();
    }
    else if (event == "task_completed") {
        entityId = *static_cast<int*>(data.get());  // Task ID
        
        auto task = TaskController::getTaskById(entityId);
        if (!task) return;
        
        type = NotificationType::TASK_COMPLETED;
        message = "Task completed: " + task->getTitle();
        userId = task->getCreator()->getId();
    }
    // Handle other event types similarly
    
    if (userId > 0) {
        storeNotification(userId, message, entityId);
    }
}

void InAppNotificationObserver::storeNotification(int userId, const std::string& message, int entityId) {
    // In a real implementation, this would store the notification in the database
    // For this example, we'll use the NotificationController
    auto user = UserController::getUserById(userId);
    if (!user) return;
    
    NotificationController notificationController;
    notificationController.createNotification(message, NotificationType::TASK_ASSIGNED, user, entityId);
}