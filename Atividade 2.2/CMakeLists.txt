cmake_minimum_required(VERSION 3.5)
project(P1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(CLIENT_SOURCE_FILES socket_helper.c client.c)
set(SERVER_SOURCE_FILES socket_helper.c server.c)

add_executable(client ${CLIENT_SOURCE_FILES})
add_executable(server ${SERVER_SOURCE_FILES})