// src/user.cpp
#include "../include/user.h"

User::User() : userId(0), role(UserRole::REGULAR) {
}

User::User(int id, const std::string& username, const std::string& password, 
          const std::string& email, UserRole role)
    : userId(id), username(username), password(password), email(email), role(role) {
}

// Getters implementation
int User::getId() const { return userId; }
std::string User::getUsername() const { return username; }
std::string User::getEmail() const { return email; }
UserRole User::getRole() const { return role; }

// Setters implementation
void User::setUsername(const std::string& newUsername) { username = newUsername; }
void User::setEmail(const std::string& newEmail) { email = newEmail; }
void User::setRole(UserRole newRole) { role = newRole; }

// Authentication implementation
bool User::validatePassword(const std::string& inputPassword) const {
    // In a real system, this would involve proper hashing and security measures
    return password == inputPassword;
}

void User::changePassword(const std::string& newPassword) {
    // In a real system, this would involve proper hashing and security measures
    password = newPassword;
}

// Utility implementation
std::string User::roleToString() const {
    switch (role) {
        case UserRole::REGULAR: return "Regular";
        case UserRole::ADMIN: return "Admin";
        case UserRole::TEAM_LEAD: return "Team Lead";
        default: return "Unknown";
    }
}