cmake_minimum_required(VERSION 3.15)
project(code C)

set(CMAKE_C_STANDARD 11)

include_directories(include)
include_directories(src)

add_executable(code
        include/array_util.h
        include/list_util.h
        src/array_util.c
        src/list_util.c
        src/sort.c)
