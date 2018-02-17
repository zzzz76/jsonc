cmake_minimum_required(VERSION 3.5)
project(cjson C)

set(CMAKE_C_STANDARD 99)

add_executable(cjson parse_value.c test.c parse_value.h)