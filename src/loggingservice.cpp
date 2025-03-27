#include "loggingservice.h"
#include <iostream>

void LoggingService::logEvent(const std::string& event) {
    std::cout << "Log: " << event << std::endl;
}
