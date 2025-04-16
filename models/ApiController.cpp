#include "../controllers/ApiController.h"
#include "../controllers/UserController.h"
#include "../controllers/ProjectController.h"
#include "../controllers/TaskController.h"
#include "../controllers/NotificationController.h"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

ApiController::ApiController() {
    registerRoutes();
}

void ApiController::registerRoutes() {
    // User routes
    routes["/api/users/login"] = [this](const Request& req) { return handleUserLogin(req); };
    routes["/api/users/register"] = [this](const Request& req) { return handleUserRegister(req); };
    routes["/api/users/profile"] = [this](const Request& req) { return handleGetUserProfile(req); };
    routes["/api/users/update"] = [this](const Request& req) { return handleUpdateUserProfile(req); };
    
    // Project routes
    routes["/api/projects"] = [this](const Request& req) { 
        if (req.method == "GET") return handleGetProjects(req);
        if (req.method == "POST") return handleCreateProject(req);
        return respondBadRequest("Method not supported");
    };
    
    routes["/api/projects/:id"] = [this](const Request& req) {
        if (req.method == "GET") return handleGetProject(req);
        if (req.method == "PUT") return handleUpdateProject(req);
        if (req.method == "DELETE") return handleDeleteProject(req);
        return respondBadRequest("Method not supported");
    };
    
    routes["/api/projects/:id/members"] = [this](const Request& req) {
        if (req.method == "POST") return handleAddProjectMember(req);
        if (req.method == "DELETE") return handleRemoveProjectMember(req);
        return respondBadRequest("Method not supported");
    };
    
    // Task routes
    routes["/api/tasks"] = [this](const Request& req) {
        if (req.method == "GET") return handleGetTasks(req);
        if (req.method == "POST") return handleCreateTask(req);
        return respondBadRequest("Method not supported");
    };
    
    routes["/api/tasks/:id"] = [this](const Request& req) {
        if (req.method == "GET") return handleGetTask(req);
        if (req.method == "PUT") return handleUpdateTask(req);
        if (req.method == "DELETE") return handleDeleteTask(req);
        return respondBadRequest("Method not supported");
    };
    
    routes["/api/tasks/:id/assign"] = [this](const Request& req) {
        if (req.method == "POST") return handleAssignTask(req);
        return respondBadRequest("Method not supported");
    };
    
    routes["/api/tasks/:id/complete"] = [this](const Request& req) {
        if (req.method == "POST") return handleCompleteTask(req);
        return respondBadRequest("Method not supported");
    };
    
    routes["/api/tasks/:id/comments"] = [this](const Request& req) {
        if (req.method == "POST") return handleAddTaskComment(req);
        return respondBadRequest("Method not supported");
    };
    
    routes["/api/tasks/:id/attachments"] = [this](const Request& req) {
        if (req.method == "POST") return handleAddTaskAttachment(req);
        return respondBadRequest("Method not supported");
    };
    
    // Notification routes
    routes["/api/notifications"] = [this](const Request& req) {
        if (req.method == "GET") return handleGetNotifications(req);
        return respondBadRequest("Method not supported");
    };
    
    routes["/api/notifications/:id/read"] = [this](const Request& req) {
        if (req.method == "POST") return handleMarkNotificationAsRead(req);
        return respondBadRequest("Method not supported");
    };
}

Response ApiController::handleRequest(const Request& request) {
    // Extract the path without query parameters
    std::string path = request.path;
    size_t queryPos = path.find('?');
    if (queryPos != std::string::npos) {
        path = path.substr(0, queryPos);
    }
    
    // Check for exact match
    if (routes.find(path) != routes.end()) {
        return routes[path](request);
    }
    
    // Check for parameterized routes (e.g., /api/projects/:id)
    for (const auto& route : routes) {
        std::string routePath = route.first;
        
        // Skip if not a parameterized route
        if (routePath.find(':') == std::string::npos) {
            continue;
        }
        
        // Convert route pattern to regex
        std::string regexPattern = routePath;
        size_t pos = 0;
        while ((pos = regexPattern.find(':', pos)) != std::string::npos) {
            size_t end = regexPattern.find('/', pos);
            if (end == std::string::npos) {
                end = regexPattern.length();
            }
            
            regexPattern.replace(pos, end - pos, "([^/]+)");
            pos += 7; // Length of "([^/]+)"
        }
        
        // Match path against pattern
        std::regex regex(regexPattern);
        std::smatch matches;
        if (std::regex_match(path, matches, regex)) {
            // Path matches pattern, execute handler
            return route.second(request);
        }
    }
    
    // No matching route found
    return respondNotFound();
}

Response ApiController::respondWithJson(const std::string& json, int statusCode) {
    Response response;
    response.statusCode = statusCode;
    response.headers["Content-Type"] = "application/json";
    response.body = json;
    return response;
}

Response ApiController::respondNotFound() {
    json errorObj = {
        {"error", "Not found"},
        {"message", "The requested resource was not found."}
    };
    return respondWithJson(errorObj.dump(), 404);
}

Response ApiController::respondUnauthorized() {
    json errorObj = {
        {"error", "Unauthorized"},
        {"message", "You need to be authenticated to access this resource."}
    };
    return respondWithJson(errorObj.dump(), 401);
}

Response ApiController::respondBadRequest(const std::string& message) {
    json errorObj = {
        {"error", "Bad request"},
        {"message", message}
    };
    return respondWithJson(errorObj.dump(), 400);
}

std::shared_ptr<User> ApiController::authenticateUser(const Request& request) {
    // Get token from Authorization header
    std::string token;
    auto it = request.headers.find("Authorization");
    if (it != request.headers.end()) {
        std::string authHeader = it->second;
        if (authHeader.substr(0, 7) == "Bearer ") {
            token = authHeader.substr(7);
        }
    }
    
    if (token.empty()) {
        return nullptr;
    }
    
    // In a real system, this would validate the token
    // For this example, we'll just assume a valid token and return a user
    // In practice, you'd want to verify the token signature and get the user ID from it
    return UserController::getUserById(1); // Return admin user
}

// Sample implementation of user login
Response ApiController::handleUserLogin(const Request& request) {
    // Parse request body
    json requestData = json::parse(request.body);
    std::string username = requestData["username"];
    std::string password = requestData["password"];
    
    // Authenticate user
    auto user = UserController::authenticateUser(username, password);
    
    if (!user) {
        return respondUnauthorized();
    }
    
    // In a real system, you'd generate a JWT token here
    std::string token = "sample_token_for_user_" + std::to_string(user->getId());
    
    json responseData = {
        {"token", token},
        {"user", json::parse(user->toJson())}
    };
    
    return respondWithJson(responseData.dump());
}