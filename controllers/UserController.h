#ifndef USER_CONTROLLER_H
#define USER_CONTROLLER_H

#include <memory>
#include <vector>
#include <string>
#include "../models/User.h"

class UserController {
public:
    // User authentication
    static std::shared_ptr<User> authenticateUser(const std::string& username, const std::string& password);
    static std::shared_ptr<User> registerUser(const std::string& username, const std::string& email, 
                                             const std::string& password, const std::string& roleName);
    
    // User management
    static std::shared_ptr<User> getUserById(int userId);
    static std::shared_ptr<User> getUserByUsername(const std::string& username);
    static std::vector<std::shared_ptr<User>> getAllUsers();
    
    static bool updateUser(std::shared_ptr<User> user);
    static bool deleteUser(int userId);
    
    // Password management
    static bool changePassword(int userId, const std::string& oldPassword, const std::string& newPassword);
    static bool resetPassword(int userId);
};

#endif // USER_CONTROLLER_H