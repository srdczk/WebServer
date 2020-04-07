cmake_minimum_required(VERSION 3.10)
project(WEB_SOCKET_SERVER)

set(CMAKE_CXX_STANDARD 17)

set(base_srcs
        base/AsyncLog.cpp
        base/ConfigFileReader.cpp
        base/Timestamp.cpp
        net/Buffer.cpp
        main.cpp
        net/EventLoop.cpp net/EventLoop.h)

add_executable(WebSocketServer ${base_srcs})

target_link_libraries(WebSocketServer pthread)