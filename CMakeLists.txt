# ------------------------------------------------------------
#  logentia/CMakeLists.txt
# ------------------------------------------------------------
cmake_minimum_required(VERSION 3.16)

project(logentia
        VERSION 1.0.0
        DESCRIPTION "Highly-configurable C++ logging protocol"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─────────────────────────────────────────────────────────────
#  Library target
# ─────────────────────────────────────────────────────────────
add_library(logentia
    src/logentia.cpp
    src/config.cpp
)

# Public headers propagate to consumers
target_include_directories(logentia PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:include>
)

# Link std-k (discoverable via find_package)
find_package(std-k CONFIG REQUIRED)
target_link_libraries(logentia PUBLIC std-k::std-k)

# ─────────────────────────────────────────────────────────────
#  Install rules
# ─────────────────────────────────────────────────────────────
include(GNUInstallDirs)

# 1. Library + headers
install(TARGETS logentia
        EXPORT logentiaTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY inc/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# 2. Export target set
install(EXPORT logentiaTargets
        FILE logentiaTargets.cmake
        NAMESPACE logentia::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/logentia)

# ─────────────────────────────────────────────────────────────
#  Package configuration files
# ─────────────────────────────────────────────────────────────
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/logentiaConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    cmake/logentiaConfig.cmake.in                # template (two lines)
    ${CMAKE_CURRENT_BINARY_DIR}/logentiaConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/logentia
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/logentiaConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/logentiaConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/logentia)

# Copyright (c) 2025, Maxamilian Kidd-May
# All rights reserved.

# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

