cmake_minimum_required(VERSION 3.20)
project(parallel_ddz)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP REQUIRED)
find_package(MPI REQUIRED)
set(CMAKE_C_COMPILER mpicc)
set(CMAKE_CXX_COMPILER mpicxx)
set(MPI_GUESS_LIBRARY_NAME MPICH2)

include_directories(SYSTEM ${MPI_INCLUDE_PATH})

add_executable(main main.cpp CmdParser.h InputParser.cpp InputParser.h)
target_link_libraries(main PUBLIC MPI::MPI_CXX)
target_link_libraries(main PUBLIC OpenMP::OpenMP_CXX)
