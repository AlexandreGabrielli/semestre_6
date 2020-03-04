cmake_minimum_required(VERSION 3.15)
project(4___z_buffering)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(4___z_buffering
        CMakeLists.txt
        geometry.h
        model.cpp
        model.h
        random_head.cpp
        tgaimage.cpp
        tgaimage.h)
