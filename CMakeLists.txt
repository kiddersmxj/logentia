cmake_minimum_required(VERSION 3.16)
project(logentia LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(logentia
    src/logentia.cpp
    src/config.cpp
)

find_package(std-k REQUIRED)

target_link_libraries(logentia
    PRIVATE std-k
)

target_include_directories(logentia
    PRIVATE inc
)

# Copyright (c) 2025, Maxamilian Kidd-May
# All rights reserved.

# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

