#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <memory>
#include <map>
#include "../controllers/ApiController.h"

class HttpServer {
private:
    int port;
    bool running;
    std::map<std::string, std::shared_ptr<ApiController>> controllers;
    
    // Parse incoming HTTP request
    Request parseRequest(const std::string& requestData);
    
    // Format HTTP response
    std::string formatResponse(const Response& response);

public:
    HttpServer(int port);
    
    // Register controller for a specific path
    void registerController(const std::string& basePath, std::shared_ptr<ApiController> controller);
    
    // Start server and listen for connections
    bool start();
    
    // Stop server
    void stop();
    
    // Handle incoming connection
    void handleConnection(int clientSocket);
    
    // Route request to appropriate controller
    Response routeRequest(const Request& request);
};

#endif // HTTP_SERVER_H