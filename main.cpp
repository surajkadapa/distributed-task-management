#include <iostream>
#include <memory>
#include "controllers/ApiController.h"
#include "config/DatabaseManager.h"
#include "server/HttpServer.h"

int main() {
    // Initialize the database
    auto& dbManager = DatabaseManager::getInstance();
    dbManager.initialize();
    
    // Create and configure HTTP server
    HttpServer server(8080);
    
    // Register API endpoints
    auto apiController = std::make_shared<ApiController>();
    server.registerController("/api", apiController);
    
    std::cout << "Starting Task Management System server on port 8080..." << std::endl;
    server.start();
    
    return 0;
}