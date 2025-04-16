#include "../config/DatabaseManager.h"
#include <iostream>

DatabaseManager::DatabaseManager() : db(nullptr), dbPath("taskmanager.db") {}

DatabaseManager::~DatabaseManager() {
    if (db) {
        sqlite3_close(db);
    }
}

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initialize() {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    createTables();
    return true;
}

void DatabaseManager::createTables() {
    // Create users table
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            email TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            role_id INTEGER NOT NULL,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (role_id) REFERENCES roles(id)
        )
    )");
    
    // Create roles table
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS roles (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        )
    )");
    
    // Create role_permissions table
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS role_permissions (
            role_id INTEGER NOT NULL,
            permission TEXT NOT NULL,
            PRIMARY KEY (role_id, permission),
            FOREIGN KEY (role_id) REFERENCES roles(id)
        )
    )");
    
    // Create projects table
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS projects (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            description TEXT,
            start_date TEXT,
            end_date TEXT,
            owner_id INTEGER NOT NULL,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (owner_id) REFERENCES users(id)
        )
    )");
    
    // Create project_members table
    // Create project_members table
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS project_members (
            project_id INTEGER NOT NULL,
            user_id INTEGER NOT NULL,
            joined_at TEXT DEFAULT CURRENT_TIMESTAMP,
            PRIMARY KEY (project_id, user_id),
            FOREIGN KEY (project_id) REFERENCES projects(id),
            FOREIGN KEY (user_id) REFERENCES users(id)
        )
    )");
    
    // Create tasks table
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS tasks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            project_id INTEGER NOT NULL,
            title TEXT NOT NULL,
            description TEXT,
            status TEXT NOT NULL,
            priority TEXT NOT NULL,
            task_type TEXT NOT NULL,
            created_date TEXT DEFAULT CURRENT_TIMESTAMP,
            due_date TEXT,
            creator_id INTEGER NOT NULL,
            assignee_id INTEGER,
            FOREIGN KEY (project_id) REFERENCES projects(id),
            FOREIGN KEY (creator_id) REFERENCES users(id),
            FOREIGN KEY (assignee_id) REFERENCES users(id)
        )
    )");
    
    // Create task_comments table
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS task_comments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            task_id INTEGER NOT NULL,
            content TEXT NOT NULL,
            created_date TEXT DEFAULT CURRENT_TIMESTAMP,
            author_id INTEGER NOT NULL,
            FOREIGN KEY (task_id) REFERENCES tasks(id),
            FOREIGN KEY (author_id) REFERENCES users(id)
        )
    )");
    
    // Create task_attachments table
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS task_attachments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            task_id INTEGER NOT NULL,
            filename TEXT NOT NULL,
            file_path TEXT NOT NULL,
            upload_date TEXT DEFAULT CURRENT_TIMESTAMP,
            uploader_id INTEGER NOT NULL,
            FOREIGN KEY (task_id) REFERENCES tasks(id),
            FOREIGN KEY (uploader_id) REFERENCES users(id)
        )
    )");
    
    // Create task_history table
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS task_history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            task_id INTEGER NOT NULL,
            entry TEXT NOT NULL,
            created_date TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (task_id) REFERENCES tasks(id)
        )
    )");
    
    // Create feature_tasks table for task-specific data
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS feature_tasks (
            task_id INTEGER PRIMARY KEY,
            feature_specification TEXT,
            FOREIGN KEY (task_id) REFERENCES tasks(id)
        )
    )");
    
    // Create bug_tasks table for task-specific data
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS bug_tasks (
            task_id INTEGER PRIMARY KEY,
            steps_to_reproduce TEXT,
            expected_behavior TEXT,
            actual_behavior TEXT,
            FOREIGN KEY (task_id) REFERENCES tasks(id)
        )
    )");
    
    // Create notifications table
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS notifications (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            message TEXT NOT NULL,
            type TEXT NOT NULL,
            read INTEGER DEFAULT 0,
            created_date TEXT DEFAULT CURRENT_TIMESTAMP,
            recipient_id INTEGER NOT NULL,
            related_entity_id INTEGER,
            FOREIGN KEY (recipient_id) REFERENCES users(id)
        )
    )");
    
    // Insert default roles
    executeQuery(R"(
        INSERT OR IGNORE INTO roles (id, name) VALUES 
        (1, 'Administrator'),
        (2, 'Project Manager'),
        (3, 'Developer'),
        (4, 'Observer')
    )");
    
    // Insert default permissions for roles
    executeQuery(R"(
        INSERT OR IGNORE INTO role_permissions (role_id, permission) VALUES 
        (1, 'CREATE_PROJECT'), (1, 'DELETE_PROJECT'), (1, 'CREATE_TASK'), 
        (1, 'ASSIGN_TASK'), (1, 'DELETE_TASK'), (1, 'VIEW_REPORTS'), (1, 'ADMIN_ACCESS'),
        (2, 'CREATE_PROJECT'), (2, 'CREATE_TASK'), (2, 'ASSIGN_TASK'), (2, 'VIEW_REPORTS'),
        (3, 'CREATE_TASK'), (3, 'ASSIGN_TASK'),
        (4, 'VIEW_REPORTS')
    )");
    
    // Insert admin user
    executeQuery(R"(
        INSERT OR IGNORE INTO users (id, username, email, password_hash, role_id) VALUES 
        (1, 'admin', 'admin@example.com', '8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918', 1)
    )");
}

sqlite3* DatabaseManager::getConnection() {
    return db;
}

bool DatabaseManager::executeQuery(const std::string& query) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

void DatabaseManager::beginTransaction() {
    executeQuery("BEGIN TRANSACTION");
}

void DatabaseManager::commitTransaction() {
    executeQuery("COMMIT");
}

void DatabaseManager::rollbackTransaction() {
    executeQuery("ROLLBACK");
}

int DatabaseManager::getLastInsertId() {
    return sqlite3_last_insert_rowid(db);
}