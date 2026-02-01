cmake_minimum_required(VERSION 3.10)
project(HTTPServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

set(SOURCE_FILES src/main.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(http-server ${SOURCE_FILES})
target_link_libraries(http-server pthread)