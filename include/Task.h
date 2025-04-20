#pragma once
#include <string>
#include <atomic>

enum class TaskStatus { Pending, Running, Completed };

class Task {
public:
    Task(int id, const std::string& name, int duration);

    // Delete copy constructor & copy assignment
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    // Allow move constructor and move assignment
    Task(Task&&) noexcept;
    Task& operator=(Task&&) noexcept;

    int getId() const;
    std::string getName() const;
    int getDuration() const;
    TaskStatus getStatus() const;

    void setStatus(TaskStatus status);

private:
    int id;
    std::string name;
    int duration;
    std::atomic<TaskStatus> status;
};
