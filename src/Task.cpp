#include "../include/Task.h"

Task::Task(int id, const std::string& name, int duration)
    : id(id), name(name), duration(duration), status(TaskStatus::Pending) {}

Task::Task(Task&& other) noexcept
    : id(other.id), name(std::move(other.name)), duration(other.duration), status(other.status.load()) {}

Task& Task::operator=(Task&& other) noexcept {
    if (this != &other) {
        id = other.id;
        name = std::move(other.name);
        duration = other.duration;
        status.store(other.status.load());
    }
    return *this;
}

int Task::getId() const { return id; }
std::string Task::getName() const { return name; }
int Task::getDuration() const { return duration; }
TaskStatus Task::getStatus() const { return status.load(); }
void Task::setStatus(TaskStatus s) { status.store(s); }
