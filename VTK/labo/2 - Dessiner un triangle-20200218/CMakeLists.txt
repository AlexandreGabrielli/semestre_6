cmake_minimum_required(VERSION 3.15)
project(TR2020)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(Triangles
        triangles.cpp
        tgaimage.cpp boite.cpp)
