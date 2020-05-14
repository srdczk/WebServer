cmake_minimum_required(VERSION 3.10)
project(WebServer)

set(CMAKE_CXX_STANDARD 17)

FILE(GLOB BASE_SOURCE "base/*.cpp")
FILE(GLOB BASE_HEADER "base/*.h")

FILE(GLOB NET_SOURCE "net/*.cpp")
FILE(GLOB NET_HEADER "net/*.h")

add_executable(WebServer
        ${BASE_SOURCE}
        ${BASE_HEADER}
        ${NET_SOURCE}
        ${NET_HEADER}
        main.cpp
        )

target_link_libraries(WebServer pthread)