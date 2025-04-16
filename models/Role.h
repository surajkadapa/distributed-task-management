#ifndef ROLE_H
#define ROLE_H

#include <string>
#include <vector>

enum class Permission {
    CREATE_PROJECT,
    DELETE_PROJECT,
    CREATE_TASK,
    ASSIGN_TASK,
    DELETE_TASK,
    VIEW_REPORTS,
    ADMIN_ACCESS
};

class Role {
private:
    int id;
    std::string name;
    std::vector<Permission> permissions;

public:
    Role(int id, const std::string& name);
    
    int getId() const;
    std::string getName() const;
    std::vector<Permission> getPermissions() const;
    
    void addPermission(Permission permission);
    void removePermission(Permission permission);
    bool hasPermission(Permission permission) const;
    
    // JSON serialization
    std::string toJson() const;
};

#endif // ROLE_H