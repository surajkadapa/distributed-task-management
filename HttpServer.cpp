#include "../server/HttpServer.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

HttpServer::HttpServer(int port) : port(port), running(false) {}

void HttpServer::registerController(const std::string& basePath, std::shared_ptr<ApiController> controller) {
    controllers[basePath] = controller;
}

bool HttpServer::start() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    
    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return false;
    }
    
    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "Set socket options failed" << std::endl;
        return false;
    }
    
    // Set address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    // Bind to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return false;
    }
    
    // Listen for connections
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return false;
    }
    
    running = true;
    std::cout << "Server started on port " << port << std::endl;
    
    // Accept connections in a loop
    while (running) {
        int client_fd;
        struct sockaddr_in client_address;
        int addrlen = sizeof(client_address);
        
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_address, 
                                (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }
        
        // Handle connection in a new thread
        std::thread([this, client_fd]() {
            this->handleConnection(client_fd);
        }).detach();
    }
    
    close(server_fd);
    return true;
}

void HttpServer::stop() {
    running = false;
}

void HttpServer::handleConnection(int clientSocket) {
    char buffer[4096] = {0};
    read(clientSocket, buffer, 4096);
    
    Request request = parseRequest(buffer);
    Response response = routeRequest(request);
    
    std::string responseStr = formatResponse(response);
    send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
    close(clientSocket);
}

Request HttpServer::parseRequest(const std::string& requestData) {
    Request request;
    std::istringstream iss(requestData);
    std::string line;
    
    // Parse request line
    if (std::getline(iss, line)) {
        std::istringstream requestLine(line);
        requestLine >> request.method;
        requestLine >> request.path;
    }
    
    // Parse headers
    while (std::getline(iss, line) && line != "\r") {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            // Trim leading spaces
            value.erase(0, value.find_first_not_of(" "));
            // Trim trailing CR if present
            if (!value.empty() && value.back() == '\r') {
                value.pop_back();
            }
            request.headers[key] = value;
        }
    }
    
    // Parse body
    std::string body;
    while (std::getline(iss, line)) {
        body += line + "\n";
    }
    request.body = body;
    
    return request;
}

Response HttpServer::routeRequest(const Request& request) {
    // Extract base path from request path
    std::string basePath = request.path.substr(0, request.path.find('/', 1));
    
    // Find controller for the base path
    auto it = controllers.find(basePath);
    if (it != controllers.end()) {
        return it->second->handleRequest(request);
    }
    
    // No controller found
    Response response;
    response.statusCode = 404;
    response.headers["Content-Type"] = "application/json";
    response.body = R"({"error":"Not found","message":"No controller found for this path"})";
    return response;
}

std::string HttpServer::formatResponse(const Response& response) {
    std::ostringstream oss;
    
    // Status line
    oss << "HTTP/1.1 " << response.statusCode;
    switch (response.statusCode) {
        case 200: oss << " OK"; break;
        case 201: oss << " Created"; break;
        case 400: oss << " Bad Request"; break;
        case 401: oss << " Unauthorized"; break;
        case 404: oss << " Not Found"; break;
        default: oss << " Unknown";
    }
    oss << "\r\n";
    
    // Headers
    for (const auto& header : response.headers) {
        oss << header.first << ": " << header.second << "\r\n";
    }
    
    // Content length
    oss << "Content-Length: " << response.body.length() << "\r\n";
    
    // End of headers
    oss << "\r\n";
    
    // Body
    oss << response.body;
    
    return oss.str();
}