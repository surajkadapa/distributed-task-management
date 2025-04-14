#include "TaskManager.h"
#include "FIFOScheduler.h"
#include "../include/crow.h"
#include <string>
#include <memory>

void add_cors_headers(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type");
}

int main() {
    crow::SimpleApp app;

    auto scheduler = std::make_unique<FIFOScheduler>();
    auto manager = std::make_shared<TaskManager>(std::move(scheduler));

    // --- CORS preflight for each route ---
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

    // --- Actual Routes ---
    CROW_ROUTE(app, "/add_node").methods("POST"_method)(
        [manager](const crow::request&, crow::response& res) {
            manager->addNode();
            res.code = 200;
            res.write("Node added");
            add_cors_headers(res);
            res.end();
        });

    CROW_ROUTE(app, "/add_task").methods("POST"_method)(
        [manager](const crow::request& req, crow::response& res) {
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
            res.code = 200;
            res.write("Task added");
            add_cors_headers(res);
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
            res = crow::response(result);
            res.code = 200;
            add_cors_headers(res);
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
                res = crow::response(result);
                res.code = 200;
                add_cors_headers(res);
                res.end();
            });
        

    app.port(18080).multithreaded().run();
}
