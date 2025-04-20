#include "../include/DatabaseManager.h"
#include "../include/TaskManager.h"
#include <iostream>
#include <chrono>
#include <thread>

DatabaseManager::DatabaseManager(const std::string& dbPath) 
    : dbPath(dbPath), db(nullptr) {}

DatabaseManager::~DatabaseManager() {
    if (db) {
        sqlite3_close(db);
    }
}

bool DatabaseManager::initialize() {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        db = nullptr;
        return false;
    }
    
    // Enable foreign keys
    char* errMsg = nullptr;
    rc = sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error enabling foreign keys: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    // Create tables if they don't exist
    const char* createTasksTable = 
        "CREATE TABLE IF NOT EXISTS tasks ("
        "id INTEGER PRIMARY KEY,"
        "name TEXT NOT NULL,"
        "duration INTEGER NOT NULL,"
        "status INTEGER DEFAULT 0,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");";
        
    const char* createNodesTable = 
        "CREATE TABLE IF NOT EXISTS nodes ("
        "id INTEGER PRIMARY KEY,"
        "task_count INTEGER DEFAULT 0,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");";
        
    const char* createTaskNodeTable = 
        "CREATE TABLE IF NOT EXISTS task_node ("
        "task_id INTEGER,"
        "node_id INTEGER,"
        "assigned_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "PRIMARY KEY (task_id, node_id),"
        "FOREIGN KEY (task_id) REFERENCES tasks(id) ON DELETE CASCADE,"
        "FOREIGN KEY (node_id) REFERENCES nodes(id) ON DELETE CASCADE"
        ");";
    
    rc = sqlite3_exec(db, createTasksTable, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error creating tasks table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    rc = sqlite3_exec(db, createNodesTable, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error creating nodes table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    rc = sqlite3_exec(db, createTaskNodeTable, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error creating task_node table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    std::cout << "Database initialized successfully." << std::endl;
    return true;
}

bool DatabaseManager::saveTask(const std::shared_ptr<Task>& task) {
    const char* sql = "INSERT OR REPLACE INTO tasks (id, name, duration, status, updated_at) "
                      "VALUES (?, ?, ?, ?, CURRENT_TIMESTAMP);";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return false;
    
    sqlite3_bind_int(stmt, 1, task->getId());
    sqlite3_bind_text(stmt, 2, task->getName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, task->getDuration());
    sqlite3_bind_int(stmt, 4, static_cast<int>(task->getStatus()));
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        logError("saveTask");
        return false;
    }
    
    return true;
}

bool DatabaseManager::updateTaskStatus(int taskId, TaskStatus status) {
    const char* sql = "UPDATE tasks SET status = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return false;
    
    sqlite3_bind_int(stmt, 1, static_cast<int>(status));
    sqlite3_bind_int(stmt, 2, taskId);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        logError("updateTaskStatus");
        return false;
    }
    
    return true;
}

std::vector<std::shared_ptr<Task>> DatabaseManager::loadAllTasks() {
    std::vector<std::shared_ptr<Task>> tasks;
    const char* sql = "SELECT id, name, duration, status FROM tasks ORDER BY id;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return tasks;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int duration = sqlite3_column_int(stmt, 2);
        TaskStatus status = static_cast<TaskStatus>(sqlite3_column_int(stmt, 3));
        
        auto task = std::make_shared<Task>(id, name, duration);
        task->setStatus(status);
        tasks.push_back(task);
    }
    
    sqlite3_finalize(stmt);
    return tasks;
}

std::shared_ptr<Task> DatabaseManager::loadTask(int taskId) {
    const char* sql = "SELECT id, name, duration, status FROM tasks WHERE id = ?;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return nullptr;
    
    sqlite3_bind_int(stmt, 1, taskId);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int duration = sqlite3_column_int(stmt, 2);
        TaskStatus status = static_cast<TaskStatus>(sqlite3_column_int(stmt, 3));
        
        auto task = std::make_shared<Task>(id, name, duration);
        task->setStatus(status);
        
        sqlite3_finalize(stmt);
        return task;
    }
    
    sqlite3_finalize(stmt);
    return nullptr;
}

bool DatabaseManager::deleteTask(int taskId) {
    const char* sql = "DELETE FROM tasks WHERE id = ?;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return false;
    
    sqlite3_bind_int(stmt, 1, taskId);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        logError("deleteTask");
        return false;
    }
    
    return true;
}

bool DatabaseManager::saveNode(const std::shared_ptr<Node>& node) {
    const char* sql = "INSERT OR REPLACE INTO nodes (id, task_count) VALUES (?, ?);";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return false;
    
    sqlite3_bind_int(stmt, 1, node->getId());
    sqlite3_bind_int(stmt, 2, node->getTaskCount());
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        logError("saveNode");
        return false;
    }
    
    return true;
}

bool DatabaseManager::updateNodeTaskCount(int nodeId, int taskCount) {
    const char* sql = "UPDATE nodes SET task_count = ? WHERE id = ?;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return false;
    
    sqlite3_bind_int(stmt, 1, taskCount);
    sqlite3_bind_int(stmt, 2, nodeId);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        logError("updateNodeTaskCount");
        return false;
    }
    
    return true;
}

std::vector<std::shared_ptr<Node>> DatabaseManager::loadAllNodes(TaskManager* manager) {
    std::vector<std::shared_ptr<Node>> nodes;
    const char* sql = "SELECT id FROM nodes ORDER BY id;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return nodes;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        
        // Create a node with a reference to the task manager
        auto node = std::make_shared<Node>(id, manager);
        nodes.push_back(node);
    }
    
    sqlite3_finalize(stmt);
    return nodes;
}

bool DatabaseManager::deleteNode(int nodeId) {
    const char* sql = "DELETE FROM nodes WHERE id = ?;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return false;
    
    sqlite3_bind_int(stmt, 1, nodeId);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        logError("deleteNode");
        return false;
    }
    
    return true;
}

bool DatabaseManager::assignTaskToNode(int taskId, int nodeId) {
    const char* sql = "INSERT OR REPLACE INTO task_node (task_id, node_id) VALUES (?, ?);";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return false;
    
    sqlite3_bind_int(stmt, 1, taskId);
    sqlite3_bind_int(stmt, 2, nodeId);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        logError("assignTaskToNode");
        return false;
    }
    
    return true;
}

bool DatabaseManager::removeTaskFromNode(int taskId, int nodeId) {
    const char* sql = "DELETE FROM task_node WHERE task_id = ? AND node_id = ?;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return false;
    
    sqlite3_bind_int(stmt, 1, taskId);
    sqlite3_bind_int(stmt, 2, nodeId);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        logError("removeTaskFromNode");
        return false;
    }
    
    return true;
}

std::vector<int> DatabaseManager::getNodeTaskIds(int nodeId) {
    std::vector<int> taskIds;
    const char* sql = "SELECT task_id FROM task_node WHERE node_id = ?;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return taskIds;
    
    sqlite3_bind_int(stmt, 1, nodeId);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int taskId = sqlite3_column_int(stmt, 0);
        taskIds.push_back(taskId);
    }
    
    sqlite3_finalize(stmt);
    return taskIds;
}

int DatabaseManager::getTaskCount() {
    const char* sql = "SELECT COUNT(*) FROM tasks;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return 0;
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int DatabaseManager::getNodeCount() {
    const char* sql = "SELECT COUNT(*) FROM nodes;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return 0;
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int DatabaseManager::getPendingTaskCount() {
    const char* sql = "SELECT COUNT(*) FROM tasks WHERE status = 0;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return 0;
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int DatabaseManager::getRunningTaskCount() {
    const char* sql = "SELECT COUNT(*) FROM tasks WHERE status = 1;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return 0;
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int DatabaseManager::getCompletedTaskCount() {
    const char* sql = "SELECT COUNT(*) FROM tasks WHERE status = 2;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return 0;
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return count;
}

int DatabaseManager::getLastInsertId() {
    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

int DatabaseManager::getMaxTaskId() {
    const char* sql = "SELECT MAX(id) FROM tasks;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return 0;
    
    int maxId = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        maxId = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return maxId;
}

int DatabaseManager::getMaxNodeId() {
    const char* sql = "SELECT MAX(id) FROM nodes;";
    
    sqlite3_stmt* stmt = prepareStatement(sql);
    if (!stmt) return 0;
    
    int maxId = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        maxId = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return maxId;
}

sqlite3_stmt* DatabaseManager::prepareStatement(const std::string& sql) {
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    
    return stmt;
}

void DatabaseManager::logError(const std::string& operation) {
    std::cerr << "SQLite error during " << operation << ": " << sqlite3_errmsg(db) << std::endl;
}