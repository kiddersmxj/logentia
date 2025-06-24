cmake_minimum_required(VERSION 3.16)
project(logentia_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(std-k CONFIG REQUIRED)
find_package(logentia CONFIG REQUIRED)

add_executable(demo main.cpp)

target_link_libraries(demo
    PRIVATE std-k::std-k
    PRIVATE logentia::logentia
)
