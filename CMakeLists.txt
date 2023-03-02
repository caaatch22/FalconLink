######################################################################################################################
# Setup
######################################################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)    # define the minimum required version of CMake to be used
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # For clang-tidy.
SET(BUILD_SHARED_LIBS OFF)              # We expect external libraries to be linked statically.
SET(CMAKE_CXX_STANDARD 17)              # Compile in C++17.
SET(CMAKE_CXX_STANDARD_REQUIRED ON)     # Require C++17 support.

project(FalconLink
        VERSION 0.1.0
        LANGUAGES C CXX
        )

# People keep running CMake in the wrong folder, completely nuking their project or creating weird bugs.
# This checks if you're running CMake from a folder that already has CMakeLists.txt.
# Importantly, this catches the common case of running it from the root directory.
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" PATH_TO_CMAKELISTS_TXT)
if (EXISTS "${PATH_TO_CMAKELISTS_TXT}")
    message(FATAL_ERROR "Run CMake from a build subdirectory! \"mkdir build ; cd build ; cmake .. \" \
    Some junk files were created in this folder (CMakeCache.txt, CMakeFiles); you should delete those.")
endif ()

# Detect the operating system
MESSAGE("Compiling on the operating system of ${CMAKE_SYSTEM_NAME}")
IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    ADD_DEFINITIONS(-DOS_LINUX)
ELSE() # TODO(catch22): support macOS and even windows
    MESSAGE(FATAL_ERROR "Your operating system ${CMAKE_SYSTEM_NAME} is not supported.")
ENDIF()

# Expected directory structure.
set(FALCON_BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build_support")
set(FALCON_CLANG_SEARCH_PATH "/usr/local/bin" "/usr/bin" "/usr/local/opt/llvm/bin" "/usr/local/opt/llvm@8/bin" "/usr/local/Cellar/llvm/8.0.1/bin")

######################################################################################################################
# DEPENDENCIES
######################################################################################################################

# clang-format
if (NOT DEFINED CLANG_FORMAT_BIN)
    # attempt to find the binary if user did not specify
    find_program(CLANG_FORMAT_BIN
            NAMES clang-format clang-format-12
            HINTS ${FALCON_CLANG_SEARCH_PATH})
endif ()
if ("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
    message(WARNING "Falconlink/main couldn't find clang-format.")
else ()
    message(STATUS "FalconLink/main found clang-format at ${CLANG_FORMAT_BIN}")
endif ()

# clang-tidy
find_program(CLANG_TIDY_BIN
        NAMES clang-tidy clang-tidy-12
        HINTS ${FALCON_CLANG_SEARCH_PATH})

if ("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
    message(WARNING "FalconLink/main couldn't find clang-tidy.")
else ()
    # Output compile_commands.json
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    message(STATUS "BusTub/main found clang-tidy at ${CLANG_TIDY_BIN}")
endif ()

# cpplint
find_program(CPPLINT_BIN
        NAMES cpplint cpplint.py
        HINTS ${FALCON_BUILD_SUPPORT_DIR})
if ("${CPPLINT_BIN}" STREQUAL "CPPLINT_BIN-NOTFOUND")
    message(WARNING "FalconLink/main couldn't find cpplint.")
else ()
    message(STATUS "FalconLink/main found cpplint at ${CPPLINT_BIN}")
endif ()

######################################################################################################################
# Include
######################################################################################################################
SET(FALCONLINK_SRC_DIR ${PROJECT_SOURCE_DIR}/falconlink)
SET(FALCONLINK_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/falconlink/include)
SET(FALCONLINK_EXAMPLE_DIR ${PROJECT_SOURCE_DIR}/examples)

######################################################################################################################
# Build
######################################################################################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -std=c++17 -pthread")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-attributes") # TODO: remove
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")

# Build the falcon net library
FILE(GLOB FALCON_NET_SOURCE RELATIVE ${CMAKE_SOURCE_DIR} "falconlink/net/*.cpp")
ADD_LIBRARY(falcon_net ${FALCON_NET_SOURCE})
TARGET_COMPILE_OPTIONS(falcon_net PRIVATE ${CMAKE_COMPILER_FLAG})
TARGET_INCLUDE_DIRECTORIES(
        falcon_net
        PUBLIC ${FALCONLINK_SRC_INCLUDE_DIR}
)

# Build the echo server
ADD_EXECUTABLE(echo_server ${FALCONLINK_EXAMPLE_DIR}/echo/echo_server.cpp)
TARGET_LINK_LIBRARIES(echo_server falcon_net)
TARGET_COMPILE_OPTIONS(echo_server PRIVATE ${CMAKE_COMPILER_FLAG})
TARGET_INCLUDE_DIRECTORIES(
        echo_server
        PUBLIC ${FALCONLINK_SRC_INCLUDE_DIR}
)

# Build the echo client
ADD_EXECUTABLE(echo_client ${FALCONLINK_EXAMPLE_DIR}/echo/echo_client.cpp)
TARGET_LINK_LIBRARIES(echo_client falcon_net)
TARGET_INCLUDE_DIRECTORIES(
        echo_client
        PUBLIC ${FALCONLINK_SRC_INCLUDE_DIR}
)

######################################################################################################################
# format stuff
######################################################################################################################

# runs clang format and updates files in place. "make format"
add_custom_target(format ${FALCON_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${FALCON_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${FALCONLINK_SRC_DIR}
        # TODO(catch22): add this after finish test unit
        # --source_dirs
        # ${FALCONLINK_TEST_DIR}
        --fix
        --quiet
        )
    
# runs clang format and exits with a non-zero exit code if any files need to be reformatted "make check-format"
add_custom_target(check-format ${FALCON_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${FALCON_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${FALCONLINK_SRC_DIR}
        --quiet
        )

# "make cpplint"
file(GLOB_RECURSE FALCONLINK_LINT_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/falcon/*.h*"
        "${CMAKE_CURRENT_SOURCE_DIR}/falcon/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.h*"
        "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp"
        # TODO(catch22): add this after finish test unit
        # "${CMAKE_CURRENT_SOURCE_DIR}/test/*.hpp"
        # "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
        )

# Balancing act: cpplint.py takes a non-trivial time to launch,
# so process 12 files per invocation, while still ensuring parallelism
add_custom_target(cpplint 
        echo '${FALCONLINK_LINT_FILES}' | xargs -n12 -P8
        ${CPPLINT_BIN}
        --verbose=2 --quiet
        --linelength=120
        --filter=-legal/copyright,-build/include_subdir,-readability/casting,-runtime/references
        )

# runs clang-tidy and exits with a non-zero exit code if any errors are found.
# note that clang-tidy automatically looks for a .clang-tidy file in parent directories
add_custom_target(clang-tidy
        ${FALCON_BUILD_SUPPORT_DIR}/run_clang_tidy.py                     # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN}                              # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR}                                            # using cmake's generated compile commands
        )
# add_dependencies(check-clang-tidy pine_shared)                    # needs gtest headers, compile_commands.json
