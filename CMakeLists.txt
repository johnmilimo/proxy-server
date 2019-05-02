cmake_minimum_required(VERSION 3.13)
project(simple_http_server C)

set(CMAKE_C_STANDARD 11)

add_executable(simple_http_server main.c tcp_server.c)