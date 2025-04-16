#ifndef TASK_COMMENT_H
#define TASK_COMMENT_H

#include <string>
#include <memory>
#include "User.h"

class TaskComment {
private:
    int id;
    std::string content;
    std::string createdDate;
    std::shared_ptr<User> author;

public:
    TaskComment(int id, const std::string& content, const std::string& createdDate,
                std::shared_ptr<User> author);
    
    int getId() const;
    std::string getContent() const;
    std::string getCreatedDate() const;
    std::shared_ptr<User> getAuthor() const;
    
    void setContent(const std::string& content);
    
    // JSON serialization
    std::string toJson() const;
};

#endif // TASK_COMMENT_H