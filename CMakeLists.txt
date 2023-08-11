cmake_minimum_required(VERSION 3.24)
project(puppet_cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(puppet_cpp main.cpp evaluator.h evaluator.cpp evaluator.h main.h)
