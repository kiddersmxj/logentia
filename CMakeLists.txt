cmake_minimum_required(VERSION 3.11)
 
# --- Fetch libs --------------------------------------------------------------
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.11)

project(logentia
  LANGUAGES CXX
  VERSION 1.0.0
)

add_executable(logentia src/logentia.cpp)

find_package(std-k REQUIRED)

target_include_directories(logentia
    PRIVATE inc
)

install(
    TARGETS logentia
    DESTINATION /usr/local/bin/
)

target_link_libraries(logentia
    PRIVATE std-k
)

target_compile_features(logentia PUBLIC cxx_std_20)

target_sources(logentia
    PRIVATE src/logentia.cpp
    PRIVATE src/config.cpp
)

# --- Install logentia.conf and copy to ~/.logentia.conf if it doesn't exist ----------------

# Define configurable variables (they can be set or overridden before invoking install)
set(PRGM_CONF_INSTALL_DIR "" 
    CACHE STRING "Directory (relative to the user home) where the logentia configuration file will be installed")
  set(PRGM_CONF_FILE_NAME ".logentia.conf" 
    CACHE STRING "The desired logentia configuration file name")
  set(PRGM_SOURCE_FILE "${CMAKE_SOURCE_DIR}/config.conf"
    CACHE FILEPATH "Path to the source logentia configuration file")

install(CODE "
    # Determine the original user
    if(DEFINED ENV{SUDO_USER} AND NOT \"\$ENV{SUDO_USER}\" STREQUAL \"root\")
        set(ORIGINAL_USER \"\$ENV{SUDO_USER}\")
    else()
        set(ORIGINAL_USER \"\$ENV{USER}\")
    endif()

    # Get the home directory of the original user
    execute_process(
        COMMAND sh -c \"getent passwd \${ORIGINAL_USER} | cut -d: -f6\"
        OUTPUT_VARIABLE USER_HOME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(NOT USER_HOME)
        message(FATAL_ERROR \"Failed to determine home directory for user '\${ORIGINAL_USER}'\")
    endif()

    # Inject the user-defined values into the install script.
    set(CONFIG_INSTALL_DIR \"${PRGM_CONF_INSTALL_DIR}\")
    set(CONFIG_FILE_NAME \"${PRGM_CONF_FILE_NAME}\")
    set(SOURCE_FILE \"${PRGM_SOURCE_FILE}\")

    # Construct the destination directory and file path.
    set(CONFIG_DIR \"\${USER_HOME}/\${CONFIG_INSTALL_DIR}\")
    set(CONFIG_FILE \"\${CONFIG_DIR}/\${CONFIG_FILE_NAME}\")

    # Ensure that the destination directory is created (including any intermediate directories)
    file(MAKE_DIRECTORY \"\${CONFIG_DIR}\")

    if(NOT EXISTS \"\${CONFIG_FILE}\")
        message(\"Installing configuration file to \${CONFIG_FILE}\")

        # Copy the source file to the destination directory with the desired file permissions.
        file(COPY \"\${SOURCE_FILE}\" DESTINATION \"\${CONFIG_DIR}\" 
             FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

        # If the source file name differs from the desired target name, then rename it accordingly.
        get_filename_component(SOURCE_BASENAME \"\${SOURCE_FILE}\" NAME)
        if(NOT \"\${SOURCE_BASENAME}\" STREQUAL \"\${CONFIG_FILE_NAME}\")
            file(RENAME \"\${CONFIG_DIR}/\${SOURCE_BASENAME}\" \"\${CONFIG_FILE}\")
        endif()
    else()
        message(\"Configuration file already exists at \${CONFIG_FILE}\")
    endif()
")

# Copyright (c) 2025, Maxamilian Kidd-May
# All rights reserved.

# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

