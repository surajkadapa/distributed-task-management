#include "../models/User.h"
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

User::User(int id, const std::string& username, const std::string& email, 
           const std::string& passwordHash, const Role& role)
    : id(id), username(username), email(email), passwordHash(passwordHash), role(role) {}

int User::getId() const {
    return id;
}

std::string User::getUsername() const {
    return username;
}

std::string User::getEmail() const {
    return email;
}

Role User::getRole() const {
    return role;
}

bool User::validatePassword(const std::string& password) const {
    // Hash the provided password
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.size());
    SHA256_Final(hash, &sha256);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    
    return ss.str() == passwordHash;
}

void User::setEmail(const std::string& email) {
    this->email = email;
}

void User::setRole(const Role& role) {
    this->role = role;
}

void User::changePassword(const std::string& oldPassword, const std::string& newPassword) {
    if (!validatePassword(oldPassword)) {
        return;
    }
    
    // Hash the new password
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, newPassword.c_str(), newPassword.size());
    SHA256_Final(hash, &sha256);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    
    passwordHash = ss.str();
}

std::string User::toJson() const {
    std::stringstream ss;
    ss << "{"
       << "\"id\":" << id << ","
       << "\"username\":\"" << username << "\","
       << "\"email\":\"" << email << "\","
       << "\"role\":" << role.toJson()
       << "}";
    return ss.str();
}