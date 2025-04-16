#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include "Role.h"

class User {
private:
    int id;
    std::string username;
    std::string email;
    std::string passwordHash;
    Role role;

public:
    User(int id, const std::string& username, const std::string& email, 
         const std::string& passwordHash, const Role& role);
    
    int getId() const;
    std::string getUsername() const;
    std::string getEmail() const;
    Role getRole() const;
    bool validatePassword(const std::string& password) const;
    
    // Setters
    void setEmail(const std::string& email);
    void setRole(const Role& role);
    void changePassword(const std::string& oldPassword, const std::string& newPassword);
    
    // JSON serialization for API responses
    std::string toJson() const;
};

#endif // USER_H