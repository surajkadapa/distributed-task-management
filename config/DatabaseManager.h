#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <memory>
#include <sqlite3.h>

class DatabaseManager {
private:
    sqlite3* db;
    std::string dbPath;
    
    // Private constructor for singleton
    DatabaseManager();
    ~DatabaseManager();
    
    // Prevent copy
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    // Database setup
    void createTables();

public:
    // Singleton instance
    static DatabaseManager& getInstance();
    
    // Database operations
    bool initialize();
    sqlite3* getConnection();
    
    // Transaction management
    void beginTransaction();
    void commitTransaction();
    void rollbackTransaction();
    
    // Utility functions
    int getLastInsertId();
    bool executeQuery(const std::string& query);
};

#endif // DATABASE_MANAGER_H