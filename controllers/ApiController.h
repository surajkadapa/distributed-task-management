#ifndef API_CONTROLLER_H
#define API_CONTROLLER_H

#include <string>
#include <map>
#include <functional>
#include <regex>
#include "../models/User.h"
#include "../models/Project.h"
#include "../models/Task.h"

// Request structure to encapsulate HTTP request data
struct Request {
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::string body;
};

// Response structure to encapsulate HTTP response data
struct Response {
    int statusCode;
    std::map<std::string, std::string> headers;
    std::string body;
};

using RequestHandler = std::function<Response(const Request&)>;

class ApiController {
private:
    std::map<std::string, RequestHandler> routes;
    
    // Helper methods for controllers
    Response respondWithJson(const std::string& json, int statusCode = 200);
    Response respondNotFound();
    Response respondUnauthorized();
    Response respondBadRequest(const std::string& message);
    std::shared_ptr<User> authenticateUser(const Request& request);

public:
    ApiController();
    
    // Register routes
    void registerRoutes();
    
    // Dispatch incoming request to the appropriate handler
    Response handleRequest(const Request& request);
    
    // User API endpoints
    Response handleUserLogin(const Request& request);
    Response handleUserRegister(const Request& request);
    Response handleGetUserProfile(const Request& request);
    Response handleUpdateUserProfile(const Request& request);
    
    // Project API endpoints
    Response handleCreateProject(const Request& request);
    Response handleGetProjects(const Request& request);
    Response handleGetProject(const Request& request);
    Response handleUpdateProject(const Request& request);
    Response handleDeleteProject(const Request& request);
    Response handleAddProjectMember(const Request& request);
    Response handleRemoveProjectMember(const Request& request);
    
    // Task API endpoints
    Response handleCreateTask(const Request& request);
    Response handleGetTasks(const Request& request);
    Response handleGetTask(const Request& request);
    Response handleUpdateTask(const Request& request);
    Response handleDeleteTask(const Request& request);
    Response handleAssignTask(const Request& request);
    Response handleCompleteTask(const Request& request);
    Response handleAddTaskComment(const Request& request);
    Response handleAddTaskAttachment(const Request& request);
    
    // Notification API endpoints
    Response handleGetNotifications(const Request& request);
    Response handleMarkNotificationAsRead(const Request& request);
};

#endif // API_CONTROLLER_H