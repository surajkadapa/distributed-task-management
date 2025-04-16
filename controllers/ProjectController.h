#ifndef PROJECT_CONTROLLER_H
#define PROJECT_CONTROLLER_H

#include <memory>
#include <vector>
#include <string>
#include "../models/Project.h"
#include "../models/User.h"

class ProjectController {
public:
    // Project CRUD operations
    static std::shared_ptr<Project> createProject(const std::string& name, 
                                                 const std::string& description,
                                                 const std::string& startDate,
                                                 const std::string& endDate,
                                                 std::shared_ptr<User> owner);
    
    static std::shared_ptr<Project> getProjectById(int projectId);
    static std::vector<std::shared_ptr<Project>> getProjectsByUser(int userId);
    static bool updateProject(std::shared_ptr<Project> project);
    static bool deleteProject(int projectId);
    
    // Project member management
    static bool addProjectMember(int projectId, int userId);
    static bool removeProjectMember(int projectId, int userId);
    static std::vector<std::shared_ptr<User>> getProjectMembers(int projectId);
    
    // Project statistics
    static int getCompletedTasksCount(int projectId);
    static int getPendingTasksCount(int projectId);
    static double getProjectProgress(int projectId);
};

#endif // PROJECT_CONTROLLER_H