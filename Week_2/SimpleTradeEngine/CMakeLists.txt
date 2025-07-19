cmake_minimum_required(VERSION 3.10)
project(SimpleTradeEngine)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include header files from include/ directory
include_directories(include)

# Add executable target with all source files
add_executable(SimpleTradeEngine
    main.cpp
    src/Order.cpp
    src/Trade.cpp
)
