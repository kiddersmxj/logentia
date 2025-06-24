cmake_minimum_required(VERSION 3.16)
project(logentia_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Pull in the library
add_subdirectory(../ build_logentia)

add_executable(demo main.cpp)
target_link_libraries(demo PRIVATE logentia)
target_include_directories(demo PRIVATE ../logentia/inc)
