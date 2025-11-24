cmake_minimum_required(VERSION 3.20)

project(darkstar)

set(CMAKE_CXX_STANDARD 20)

add_executable(darkstar src/main.cpp)

if (MSVC)
    target_compile_options(darkstar PRIVATE /W4)
else ()
    target_compile_options(darkstar PRIVATE -Wall -Wextra -Wpedantic)
endif ()