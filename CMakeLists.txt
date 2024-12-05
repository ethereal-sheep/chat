cmake_minimum_required(VERSION 3.10)

project(Chat)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS system)

include_directories(include)

add_executable(server src/server.cpp src/chat_server.cpp src/chat_common.cpp)
add_executable(client src/client.cpp src/chat_client.cpp src/chat_common.cpp)

target_link_libraries(server Boost::system)
target_link_libraries(client Boost::system)
