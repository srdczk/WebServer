cmake_minimum_required(VERSION 3.10)
project(WebServer)

set(CMAKE_CXX_STANDARD 17)

FILE(GLOB SERVER_SOURCE "*.cpp")
FILE(GLOB SERVER_HEADER "*.h")


add_executable(WebServer
        ${SERVER_SOURCE}
        ${SERVER_HEADER}
        )
# add pthread
target_link_libraries(WebServer pthread)