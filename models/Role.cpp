#include "../models/Role.h"
#include <sstream>
#include <algorithm>

Role::Role(int id, const std::string& name) : id(id), name(name) {}

int Role::getId() const {
    return id;
}

std::string Role::getName() const {
    return name;
}

std::vector<Permission> Role::getPermissions() const {
    return permissions;
}

void Role::addPermission(Permission permission) {
    if (std::find(permissions.begin(), permissions.end(), permission) == permissions.end()) {
        permissions.push_back(permission);
    }
}

void Role::removePermission(Permission permission) {
    permissions.erase(
        std::remove(permissions.begin(), permissions.end(), permission),
        permissions.end()
    );
}

bool Role::hasPermission(Permission permission) const {
    return std::find(permissions.begin(), permissions.end(), permission) != permissions.end();
}

std::string Role::toJson() const {
    std::stringstream ss;
    ss << "{"
       << "\"id\":" << id << ","
       << "\"name\":\"" << name << "\","
       << "\"permissions\":[";
    
    for (size_t i = 0; i < permissions.size(); i++) {
        if (i > 0) {
            ss << ",";
        }
        ss << static_cast<int>(permissions[i]);
    }
    
    ss << "]}";
    return ss.str();
}