#ifndef NOTIFICATION_CONTROLLER_H
#define NOTIFICATION_CONTROLLER_H

#include <memory>
#include <vector>
#include "../models/Notification.h"
#include "../models/User.h"
#include "../observers/NotificationObserver.h"

class NotificationController {
private:
    std::vector<std::shared_ptr<NotificationObserver>> observers;

public:
    // Observer pattern methods
    void registerObserver(std::shared_ptr<NotificationObserver> observer);
    void removeObserver(std::shared_ptr<NotificationObserver> observer);
    void notifyObservers(const std::string& event, std::shared_ptr<void> data);
    
    // Notification management
    Notification createNotification(const std::string& message, 
                                   NotificationType type,
                                   std::shared_ptr<User> recipient,
                                   int relatedEntityId);
    
    std::vector<Notification> getUserNotifications(int userId, bool unreadOnly = false);
    bool markNotificationAsRead(int notificationId);
    bool deleteNotification(int notificationId);
    
    // Event handlers
    void onTaskAssigned(int taskId, int assigneeId);
    void onTaskCompleted(int taskId);
    void onTaskCommentAdded(int taskId, int commentId);
    void onDueDateApproaching(int taskId);
    void onTaskOverdue(int taskId);
    void onProjectInvitation(int projectId, int userId);
};

#endif // NOTIFICATION_CONTROLLER_H