cmake_minimum_required(VERSION 3.5)
project(jsonc C)

set(CMAKE_C_STANDARD 99)

add_executable(jsonc jsonc.c test.c jsonc.h)