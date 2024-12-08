cmake_minimum_required(VERSION 3.24)

project(Chat)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(BOOST_INCLUDE_LIBRARIES system)
# set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)

# FetchContent_Declare(
#   Boost
#   GIT_REPOSITORY https://github.com/boostorg/boost.git
#   GIT_TAG boost-1.86.0
#   GIT_SHALLOW TRUE
# )
# FetchContent_MakeAvailable(Boost)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()
find_package(Boost REQUIRED COMPONENTS system)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2
  GIT_SHALLOW TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(include)

# bin
add_executable(chat src/chat.cpp src/chat_client.cpp src/chat_server.cpp src/chat_common.cpp src/message.cpp src/terminal.cpp)
target_link_libraries(chat Boost::system)

# test
enable_testing()

add_executable(test_chat test/test_message.cpp src/message.cpp)
target_link_libraries(test_chat GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_chat)
