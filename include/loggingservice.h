#ifndef LOGGING_SERVICE_H
#define LOGGING_SERVICE_H

#include <string>
#include <vector>
#include <fstream>
#include <mutex>

// Log levels in increasing order of severity
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

// Singleton logger class
class LoggingService {
private:
    static LoggingService* instance;
    
    std::vector<std::string> logs;
    std::ofstream logFile;
    LogLevel minLevel;
    mutable std::mutex logMutex;  // Add this line to declare the mutex
    
    // Private constructor for singleton
    LoggingService();
    
public:
    // Delete copy constructor and assignment operator
    LoggingService(const LoggingService&) = delete;
    LoggingService& operator=(const LoggingService&) = delete;
    
    // Destructor
    ~LoggingService();
    
    // Get singleton instance
    static LoggingService* getInstance();
    
    // Log methods
    void log(LogLevel level, const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);
    
    // Configuration
    void setMinLogLevel(LogLevel level);
    void setLogFile(const std::string& filename);
    
    // Retrieval
    std::vector<std::string> getRecentLogs(int count) const;
    
    // Management
    void clearLogs();
};

#endif // LOGGING_SERVICE_H