cmake_minimum_required(VERSION 3.15)
project(cuddly_potato)

set(CMAKE_CXX_STANDARD 17)

add_executable(cuddly_potato src/main.cpp src/scan.cpp src/scan.h src/task.cpp src/task.h src/argo.cpp src/argo.h src/research.cpp src/research.h src/file_type.h src/file_type.cpp src/file_type.h)