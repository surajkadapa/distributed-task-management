cmake_minimum_required(VERSION 3.10)
project(DistributedTaskManager)

set(CMAKE_CXX_STANDARD 17)

# Allow missing symbols (use with caution)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -undefined dynamic_lookup")

# Include directories
include_directories(
    config
    controllers
    models
    observers
    server
    strategies
)

# Gather all source files
set(SOURCES
    main.cpp
    models/ApiController.cpp
    models/Role.cpp
    models/User.cpp
    observers/NotificationObserver.cpp
    strategies/TaskPriorityStrategy.cpp
    DatabaseManager.cpp
    HttpServer.cpp

)

# Add the executable first
add_executable(TaskManager ${SOURCES})

# Now link the libraries to the target
find_package(nlohmann_json REQUIRED)
target_link_libraries(TaskManager PRIVATE nlohmann_json::nlohmann_json)

find_package(OpenSSL REQUIRED)
target_link_libraries(TaskManager PRIVATE OpenSSL::Crypto)

# Set paths for SQLite3 manually
set(SQLITE3_INCLUDE_DIR /opt/homebrew/opt/sqlite3/include)
set(SQLITE3_LIBRARIES /opt/homebrew/opt/sqlite3/lib/libsqlite3.dylib)

# Include the directory for SQLite3
include_directories(${SQLITE3_INCLUDE_DIR})

# Link SQLite3 directly using its path
target_link_libraries(TaskManager PRIVATE ${SQLITE3_LIBRARIES})
