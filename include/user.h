// include/user.h
#ifndef USER_H
#define USER_H

#include <string>
#include <vector>

enum class UserRole { REGULAR, ADMIN, TEAM_LEAD };

class User {
private:
    int userId;
    std::string username;
    std::string password; // In real systems, store password hash
    std::string email;
    UserRole role;

public:
    User();
    User(int id, const std::string& username, const std::string& password, 
         const std::string& email, UserRole role);
    
    // Getters
    int getId() const;
    std::string getUsername() const;
    std::string getEmail() const;
    UserRole getRole() const;
    
    // Setters
    void setUsername(const std::string& username);
    void setEmail(const std::string& email);
    void setRole(UserRole role);
    
    // Authentication
    bool validatePassword(const std::string& password) const;
    void changePassword(const std::string& newPassword);
    
    // Utility
    std::string roleToString() const;
};

#endif // USER_H