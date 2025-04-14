#include "TaskManager.h"
#include "FIFOScheduler.h"
#include <crow.h>
#include <string>
#include <memory>

// CORS Middleware to handle headers globally
struct CORSMiddleware {
    struct context {};

    void before_handle(crow::request& /*req*/, crow::response& res, context& /*ctx*/) {
        // Set CORS headers in before_handle
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
    }

    void after_handle(crow::request& /*req*/, crow::response& res, context& /*ctx*/) {
        // We need to set headers here too, in case the response was replaced
        if (!res.get_header_value("Access-Control-Allow-Origin").size()) {
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type");
        }
    }
};

int main() {
    // Use crow::App with CORSMiddleware instead of SimpleApp
    crow::App<CORSMiddleware> app;
    
    // Enable debug logging for troubleshooting
    crow::logger::setLogLevel(crow::LogLevel::Debug);

    // Initialize scheduler and task manager
    auto scheduler = std::make_unique<FIFOScheduler>();
    auto manager = std::make_shared<TaskManager>(std::move(scheduler));

    // --- CORS Preflight Routes (OPTIONS) ---
    CROW_ROUTE(app, "/add_node").methods("OPTIONS"_method)(
        [](const crow::request&, crow::response& res) {
            CROW_LOG_INFO << "Handling OPTIONS for /add_node";
            res.code = 200;
            res.end();
        });

    CROW_ROUTE(app, "/add_task").methods("OPTIONS"_method)(
        [](const crow::request&, crow::response& res) {
            CROW_LOG_INFO << "Handling OPTIONS for /add_task";
            res.code = 200;
            res.end();
        });

    CROW_ROUTE(app, "/tasks").methods("OPTIONS"_method)(
        [](const crow::request&, crow::response& res) {
            CROW_LOG_INFO << "Handling OPTIONS for /tasks";
            res.code = 200;
            res.end();
        });

    CROW_ROUTE(app, "/nodes").methods("OPTIONS"_method)(
        [](const crow::request&, crow::response& res) {
            CROW_LOG_INFO << "Handling OPTIONS for /nodes";
            res.code = 200;
            res.end();
        });

    // --- Actual Routes ---
    CROW_ROUTE(app, "/add_node").methods("POST"_method)(
        [manager](const crow::request&, crow::response& res) {
            manager->addNode();
            // Return JSON instead of plain text
            crow::json::wvalue result;
            result["success"] = true;
            result["message"] = "Node added";
            res.code = 200;
            res.write(result.dump());
            res.end();
        });

    CROW_ROUTE(app, "/add_task").methods("POST"_method)(
        [manager](const crow::request& req, crow::response& res) {
            auto body = crow::json::load(req.body);
            if (!body) {
                crow::json::wvalue error;
                error["success"] = false;
                error["message"] = "Invalid JSON";
                res.code = 400;
                res.write(error.dump());
                res.end();
                return;
            }

            std::string name = body["name"].s();
            int duration = body["duration"].i();
            manager->addTask(name, duration);
            
            // Return JSON instead of plain text
            crow::json::wvalue result;
            result["success"] = true;
            result["message"] = "Task added";
            res.code = 200;
            res.write(result.dump());
            res.end();
        });

    CROW_ROUTE(app, "/tasks").methods("GET"_method)(
        [manager](const crow::request&, crow::response& res) {
            auto tasks = manager->getAllTasks();
            crow::json::wvalue result;
            int i = 0;
            for (const auto& task : tasks) {
                result[i]["name"] = task->getName();
                result[i]["duration"] = task->getDuration();
                result[i]["status"] = static_cast<int>(task->getStatus());
                i++;
            }
            
            // Don't create a new response object, use the existing one
            res.code = 200;
            res.write(result.dump());
            res.end();
        });

    CROW_ROUTE(app, "/nodes").methods("GET"_method)(
        [manager](const crow::request&, crow::response& res) {
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
            
            // Don't create a new response object, use the existing one
            res.code = 200;
            res.write(result.dump());
            res.end();
        });

    // Run the server
    app.port(18080).multithreaded().run();
    return 0;
}