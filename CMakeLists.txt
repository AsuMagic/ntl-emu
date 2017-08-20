cmake_minimum_required(VERSION 2.8)

project(ntl_emulator)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -fno-exceptions")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DNTL_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto")

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE sources "main.cpp" "ntl/*.cpp" "ntl/*.hpp" "ntl/*.inl")
add_executable(${PROJECT_NAME} ${sources})
