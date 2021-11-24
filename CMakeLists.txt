cmake_minimum_required(VERSION 3.20)
project(partial_sum)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP REQUIRED)
find_package(MPI REQUIRED)
set(CMAKE_C_COMPILER mpicc)
set(CMAKE_CXX_COMPILER mpicxx)
set(MPI_GUESS_LIBRARY_NAME MPICH2)

include_directories(SYSTEM ${MPI_INCLUDE_PATH})

add_executable(partial_sum main.cpp CmdParser.h Sequence.cpp Sequence.h InputData.cpp InputData.h)
target_link_libraries(partial_sum PUBLIC MPI::MPI_CXX)
target_link_libraries(partial_sum PUBLIC OpenMP::OpenMP_CXX)
