cmake_minimum_required(VERSION 3.10)
project(Multi_client_TCP_Echo_Server_using_Select)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include header directory
include_directories(include)

# Server executable
add_executable(server
    src/main_server.cpp
    src/Server.cpp
    src/Utils.cpp
    include/Server.h
    include/Utils.h
)

# Client executable
add_executable(client
    src/main_client.cpp
    src/Client.cpp
    src/Utils.cpp
    include/Client.h
    include/Utils.h
)

# Link with pthread if on UNIX/Linux
if(UNIX)
    target_link_libraries(server PRIVATE pthread)
    target_link_libraries(client PRIVATE pthread)
endif()
