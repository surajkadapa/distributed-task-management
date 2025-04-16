#ifndef NOTIFICATION_OBSERVER_H
#define NOTIFICATION_OBSERVER_H

#include <string>
#include <memory>

class NotificationObserver {
public:
    virtual ~NotificationObserver() = default;
    virtual void update(const std::string& event, std::shared_ptr<void> data) = 0;
};

// Email notification observer
class EmailNotificationObserver : public NotificationObserver {
public:
    void update(const std::string& event, std::shared_ptr<void> data) override;
    
private:
    void sendEmail(const std::string& recipient, const std::string& subject, const std::string& body);
};

// In-app notification observer
class InAppNotificationObserver : public NotificationObserver {
public:
    void update(const std::string& event, std::shared_ptr<void> data) override;
    
private:
    void storeNotification(int userId, const std::string& message, int entityId);
};

#endif // NOTIFICATION_OBSERVER_H