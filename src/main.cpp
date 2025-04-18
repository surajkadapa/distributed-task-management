#include "TaskManager.h"
#include "FIFOScheduler.h"
#include "../include/crow.h"
#include "Node.h"
#include <string>
#include <memory>
#include <signal.h>
#include <atomic>

// Global flag for clean shutdown
std::atomic<bool> should_exit(false);
crow::SimpleApp* app_ptr = nullptr;

// Signal handler for clean shutdown
void signal_handler(int signal) {
    std::cout << "Received signal " << signal << ", initiating clean shutdown..." << std::endl;
    should_exit = true;
    
    // Stop the Crow app if it's running
    if (app_ptr) {
        std::cout << "Stopping Crow server..." << std::endl;
        app_ptr->stop();
    }
}

void add_cors_headers(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type");
}

int main() {
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Create Crow app
    crow::SimpleApp app;
    app_ptr = &app;
    
    // Initialize TaskManager with database
    auto scheduler = std::make_unique<FIFOScheduler>();
    auto manager = std::make_shared<TaskManager>(std::move(scheduler), "taskmaster.db");
    
    // Initialize the TaskManager (load from database)
    if (!manager->initialize()) {
        std::cerr << "Failed to initialize TaskManager with database" << std::endl;
        return 1;
    }

    // --- CORS preflight handlers ---
    CROW_ROUTE(app, "/add_node").methods("OPTIONS"_method)(
        [](const crow::request&, crow::response& res) {
            res.code = 200;
            add_cors_headers(res);
            res.end();
        });

    CROW_ROUTE(app, "/add_task").methods("OPTIONS"_method)(
        [](const crow::request&, crow::response& res) {
            res.code = 200;
            add_cors_headers(res);
            res.end();
        });

    CROW_ROUTE(app, "/tasks").methods("OPTIONS"_method)(
        [](const crow::request&, crow::response& res) {
            res.code = 200;
            add_cors_headers(res);
            res.end();
        });

    CROW_ROUTE(app, "/nodes").methods("OPTIONS"_method)(
        [](const crow::request&, crow::response& res) {
            res.code = 200;
            add_cors_headers(res);
            res.end();
        });

    CROW_ROUTE(app, "/set_scheduler").methods("OPTIONS"_method)(
        [](const crow::request&, crow::response& res) {
            res.code = 200;
            add_cors_headers(res);
            res.end();
        });
        
    CROW_ROUTE(app, "/scheduler_info").methods("OPTIONS"_method)(
        [](const crow::request&, crow::response& res) {
            res.code = 200;
            add_cors_headers(res);
            res.end();
        });
        
    CROW_ROUTE(app, "/db_stats").methods("OPTIONS"_method)(
        [](const crow::request&, crow::response& res) {
            res.code = 200;
            add_cors_headers(res);
            res.end();
        });

    // --- New Route for remove_node ---
    CROW_ROUTE(app, "/remove_node").methods("POST"_method)(
        [manager](const crow::request& req, crow::response& res) {
            try {
                auto body = crow::json::load(req.body);
                if (!body) {
                    res.code = 400;
                    res.write("Invalid JSON");
                    add_cors_headers(res);
                    res.end();
                    return;
                }

                int nodeId = body["node_id"].i();
                manager->removeNode(nodeId);
                res.code = 200;
                res.write("Node removed");
                add_cors_headers(res);
                res.end();
            } catch (const std::exception& e) {
                res.code = 500;
                res.write(std::string("Error removing node: ") + e.what());
                add_cors_headers(res);
                res.end();
            }
        });


    // --- Actual Routes ---
    CROW_ROUTE(app, "/add_node").methods("POST"_method)(
        [manager](const crow::request&, crow::response& res) {
            try {
                manager->addNode();
                
                // Return the newly created node ID
                auto nodes = manager->getAllNodes();
                int newNodeId = nodes.empty() ? 0 : nodes.back()->getId();
                
                crow::json::wvalue result;
                result["message"] = "Node added";
                result["node_id"] = newNodeId;
                
                res = crow::response(result);
                res.code = 200;
                add_cors_headers(res);
                res.end();
            } catch (const std::exception& e) {
                res.code = 500;
                res.write(std::string("Error adding node: ") + e.what());
                add_cors_headers(res);
                res.end();
            }
        });

    CROW_ROUTE(app, "/add_task").methods("POST"_method)(
        [manager](const crow::request& req, crow::response& res) {
            try {
                auto body = crow::json::load(req.body);
                if (!body) {
                    res.code = 400;
                    res.write("Invalid JSON");
                    add_cors_headers(res);
                    res.end();
                    return;
                }

                std::string name = body["name"].s();
                int duration = body["duration"].i();
                manager->addTask(name, duration);
                
                // Return success message
                crow::json::wvalue result;
                result["message"] = "Task added successfully";
                result["name"] = name;
                result["duration"] = duration;
                
                res = crow::response(result);
                res.code = 200;
                add_cors_headers(res);
                res.end();
            } catch (const std::exception& e) {
                res.code = 500;
                res.write(std::string("Error adding task: ") + e.what());
                add_cors_headers(res);
                res.end();
            }
        });

    CROW_ROUTE(app, "/tasks").methods("GET"_method)(
        [manager](const crow::request&, crow::response& res) {
            try {
                auto tasks = manager->getAllTasks();
                crow::json::wvalue result;
                int i = 0;
                for (const auto& task : tasks) {
                    result[i]["id"] = task->getId();
                    result[i]["name"] = task->getName();
                    result[i]["duration"] = task->getDuration();
                    result[i]["status"] = static_cast<int>(task->getStatus());
                    i++;
                }
                res = crow::response(result);
                res.code = 200;
                add_cors_headers(res);
                res.end();
            } catch (const std::exception& e) {
                res.code = 500;
                res.write(std::string("Error fetching tasks: ") + e.what());
                add_cors_headers(res);
                res.end();
            }
        });

    CROW_ROUTE(app, "/nodes").methods("GET"_method)(
        [manager](const crow::request&, crow::response& res) {
            try {
                auto nodes = manager->getAllNodes();
                crow::json::wvalue result;
                int i = 0;
                for (const auto& node : nodes) {
                    result[i]["id"] = node->getId();
                    result[i]["task_count"] = node->getTaskCount();
            
                    auto taskIDs = node->getTaskIDs();
                    for (size_t j = 0; j < taskIDs.size(); ++j) {
                        result[i]["task_ids"][j] = taskIDs[j];
                    }
                    i++;
                }
                res = crow::response(result);
                res.code = 200;
                add_cors_headers(res);
                res.end();
            } catch (const std::exception& e) {
                res.code = 500;
                res.write(std::string("Error fetching nodes: ") + e.what());
                add_cors_headers(res);
                res.end();
            }
        });

    CROW_ROUTE(app, "/set_scheduler").methods("POST"_method)(
        [manager](const crow::request& req, crow::response& res) {
            try {
                auto body = crow::json::load(req.body);
                if (!body) {
                    res.code = 400;
                    res.write("Invalid JSON");
                    add_cors_headers(res);
                    res.end();
                    return;
                }
        
                std::string schedulerType = body["type"].s();
                SchedulerType type;
                
                std::cout << "Processing scheduler change to: " << schedulerType << std::endl;
                
                if (schedulerType == "fifo") {
                    type = SchedulerType::FIFO;
                } else if (schedulerType == "roundrobin") {
                    type = SchedulerType::RoundRobin;
                } else if (schedulerType == "loadbalanced") {
                    type = SchedulerType::LoadBalanced;
                } else {
                    res.code = 400;
                    res.write("Invalid scheduler type");
                    add_cors_headers(res);
                    res.end();
                    return;
                }
                
                // Perform the scheduler change
                manager->setScheduler(type);
                
                // Create response
                crow::json::wvalue result;
                result["message"] = "Scheduler updated";
                result["type"] = schedulerType;
                result["name"] = manager->getCurrentSchedulerName();
                
                // Set the response directly
                res = crow::response(result);
                res.code = 200;
                add_cors_headers(res);
                
                std::cout << "Sending response for scheduler change" << std::endl;
                res.end();  // This is critical!
            } catch (const std::exception& e) {
                std::cerr << "Exception in set_scheduler: " << e.what() << std::endl;
                res.code = 500;
                res.write(std::string("Error changing scheduler: ") + e.what());
                add_cors_headers(res);
                res.end();
            } catch (...) {
                std::cerr << "Unknown exception in set_scheduler" << std::endl;
                res.code = 500;
                res.write("Unknown error changing scheduler");
                add_cors_headers(res);
                res.end();
            }
        });
        
    CROW_ROUTE(app, "/scheduler_info").methods("GET"_method)(
        [manager](const crow::request&, crow::response& res) {
            try {
                SchedulerType type = manager->getCurrentSchedulerType();
                std::string typeName;
                
                switch (type) {
                    case SchedulerType::FIFO:
                        typeName = "fifo";
                        break;
                    case SchedulerType::RoundRobin:
                        typeName = "roundrobin";
                        break;
                    case SchedulerType::LoadBalanced:
                        typeName = "loadbalanced";
                        break;
                }
                
                crow::json::wvalue result;
                result["type"] = typeName;
                result["name"] = manager->getCurrentSchedulerName();
                
                // Set the response directly
                res = crow::response(result);
                res.code = 200;
                add_cors_headers(res);
                res.end();
            } catch (const std::exception& e) {
                res.code = 500;
                res.write(std::string("Error getting scheduler info: ") + e.what());
                add_cors_headers(res);
                res.end();
            }
        });

    // New route for database statistics
    CROW_ROUTE(app, "/db_stats").methods("GET"_method)(
        [manager](const crow::request&, crow::response& res) {
            try {
                crow::json::wvalue result;
                result["total_tasks"] = manager->getTotalTaskCount();
                result["pending_tasks"] = manager->getPendingTaskCount();
                result["running_tasks"] = manager->getRunningTaskCount();
                result["completed_tasks"] = manager->getCompletedTaskCount();
                result["total_nodes"] = manager->getTotalNodeCount();
                
                // Set the response
                res = crow::response(result);
                res.code = 200;
                add_cors_headers(res);
                res.end();
            } catch (const std::exception& e) {
                res.code = 500;
                res.write(std::string("Error getting database stats: ") + e.what());
                add_cors_headers(res);
                res.end();
            }
        });

    // Add health check endpoint
    CROW_ROUTE(app, "/health").methods("GET"_method)(
        [](const crow::request&, crow::response& res) {
            res.code = 200;
            res.write("{\"status\":\"ok\"}");
            add_cors_headers(res);
            res.end();
        });

    std::cout << "Starting Crow server on port 18080..." << std::endl;
    
    // Run the server
    app.port(18080).multithreaded().run();
    
    std::cout << "Crow server stopped" << std::endl;
    std::cout << "Cleaning up resources..." << std::endl;
    
    // Allow time for cleanup before exiting
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    std::cout << "Exiting cleanly" << std::endl;
    return 0;
}