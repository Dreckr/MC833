cmake_minimum_required(VERSION 3.7)
project(P2)

set(CMAKE_C_STANDARD 99)

set(CLIENT_SOURCE_FILES client.c common.c)
set(SERVER_SOURCE_FILES server.c common.c)

add_executable(client ${CLIENT_SOURCE_FILES})
add_executable(server ${SERVER_SOURCE_FILES})
