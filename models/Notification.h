#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>
#include <memory>
#include "User.h"

enum class NotificationType {
    TASK_ASSIGNED,
    TASK_COMPLETED,
    COMMENT_ADDED,
    DUE_DATE_APPROACHING,
    TASK_OVERDUE,
    PROJECT_INVITATION
};

class Notification {
private:
    int id;
    std::string message;
    NotificationType type;
    bool read;
    std::string createdDate;
    std::shared_ptr<User> recipient;
    int relatedEntityId; // Task ID, Project ID, etc.

public:
    Notification(int id, const std::string& message, NotificationType type,
                 std::shared_ptr<User> recipient, int relatedEntityId);
    
    int getId() const;
    std::string getMessage() const;
    NotificationType getType() const;
    bool isRead() const;
    std::string getCreatedDate() const;
    std::shared_ptr<User> getRecipient() const;
    int getRelatedEntityId() const;
    
    void markAsRead();
    
    // JSON serialization
    std::string toJson() const;
};

#endif // NOTIFICATION_H