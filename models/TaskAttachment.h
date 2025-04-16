#ifndef TASK_ATTACHMENT_H
#define TASK_ATTACHMENT_H

#include <string>
#include <memory>
#include "User.h"

class TaskAttachment {
private:
    int id;
    std::string filename;
    std::string filePath;
    std::string uploadDate;
    std::shared_ptr<User> uploader;

public:
    TaskAttachment(int id, const std::string& filename, const std::string& filePath,
                   const std::string& uploadDate, std::shared_ptr<User> uploader);
    
    int getId() const;
    std::string getFilename() const;
    std::string getFilePath() const;
    std::string getUploadDate() const;
    std::shared_ptr<User> getUploader() const;
    
    // JSON serialization
    std::string toJson() const;
};

#endif // TASK_ATTACHMENT_H