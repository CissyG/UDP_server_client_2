cmake_minimum_required(VERSION 3.15)
project(COEN233_project2 C)

set(CMAKE_C_STANDARD 99)

add_executable(server server.c myPacket.c)
add_executable(client client.c myPacket.c)