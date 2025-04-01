// src/loggingservice.cpp
#include "../include/loggingservice.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

// Initialize static instance pointer
LoggingService* LoggingService::instance = nullptr;

// Singleton implementation
LoggingService* LoggingService::getInstance() {
    if (instance == nullptr) {
        instance = new LoggingService();
    }
    return instance;
}

LoggingService::LoggingService() : minLevel(LogLevel::INFO) {
    // Initialize with empty log
}

LoggingService::~LoggingService() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void LoggingService::log(LogLevel level, const std::string& message) {
    if (level < minLevel) {
        return;
    }
    
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    // Convert level to string
    std::string levelStr;
    switch (level) {
        case LogLevel::DEBUG: levelStr = "DEBUG"; break;
        case LogLevel::INFO: levelStr = "INFO"; break;
        case LogLevel::WARNING: levelStr = "WARNING"; break;
        case LogLevel::ERROR: levelStr = "ERROR"; break;
        case LogLevel::CRITICAL: levelStr = "CRITICAL"; break;
        default: levelStr = "UNKNOWN"; break;
    }
    
    // Format log message
    std::string logMessage = ss.str() + " [" + levelStr + "] " + message;
    
    // Lock for thread safety
    std::lock_guard<std::mutex> lock(logMutex);
    
    // Add to in-memory logs
    logs.push_back(logMessage);
    
    // Print to console
    std::cout << logMessage << std::endl;
    
    // Write to file if open
    if (logFile.is_open()) {
        logFile << logMessage << std::endl;
        logFile.flush();
    }
}

void LoggingService::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void LoggingService::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void LoggingService::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void LoggingService::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void LoggingService::critical(const std::string& message) {
    log(LogLevel::CRITICAL, message);
}

void LoggingService::setMinLogLevel(LogLevel level) {
    minLevel = level;
}

void LoggingService::setLogFile(const std::string& filename) {
    if (logFile.is_open()) {
        logFile.close();
    }
    
    logFile.open(filename, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

std::vector<std::string> LoggingService::getRecentLogs(int count) const {
    std::lock_guard<std::mutex> lock(logMutex);
    
    if (count <= 0 || logs.empty()) {
        return {};
    }
    
    if (count >= logs.size()) {
        return logs;
    }
    
    return std::vector<std::string>(logs.end() - count, logs.end());
}

void LoggingService::clearLogs() {
    std::lock_guard<std::mutex> lock(logMutex);
    logs.clear();
}