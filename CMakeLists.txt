cmake_minimum_required(VERSION 3.15)
project(Overlook_System C)

set(CMAKE_C_STANDARD 99)

add_executable(Overlook_System main.c fileParser.c fileParser.h Jack/mainJack.c connectionUtils/socket.c connectionUtils/socket.h)