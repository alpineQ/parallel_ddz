cmake_minimum_required(VERSION 3.20)
project(parallel_ddz)

set(CMAKE_CXX_STANDARD 17)

add_executable(main.cpp CmdParser.h InputParser.cpp InputParser.h)
