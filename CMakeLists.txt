CMAKE_MINIMUM_REQUIRED(VERSION 3.10)    # define the minimum required version of CMake to be used
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # For clang-tidy.
SET(BUILD_SHARED_LIBS OFF)              # We expect external libraries to be linked statically.
SET(CMAKE_CXX_STANDARD 17)              # Compile in C++17.
SET(CMAKE_CXX_STANDARD_REQUIRED ON)     # Require C++17 support.

project(FalconLink
        VERSION 0.5.0
        DESCRIPTION "A network libary for study"
        LANGUAGES C CXX
        )


if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to `Debug` as none was specified.")
    set(CMAKE_BUILD_TYPE "Debug")
endif ()

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
set(FALCONLINK_BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build_support")
set(FALCONLINK_CLANG_SEARCH_PATH "/usr/local/bin" "/usr/bin" "/usr/local/opt/llvm/bin" "/usr/local/opt/llvm@8/bin" "/usr/local/Cellar/llvm/8.0.1/bin")

######################################################################################################################
# DEPENDENCIES
######################################################################################################################

# CTest
enable_testing()

# clang-format

# attempt to find the binary if user did not specify
if (NOT DEFINED CLANG_FORMAT_BIN)
    # attempt to find the binary if user did not specify
    find_program(CLANG_FORMAT_BIN
            NAMES clang-format clang-format-12
            HINTS ${FALCONLINK_CLANG_SEARCH_PATH})
endif ()
if ("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
    message(WARNING "FalconLink/main couldn't find clang-format.")
else ()
    message(STATUS "FalconLink/main found clang-format at ${CLANG_FORMAT_BIN}")
endif ()

# attempt to find the binary if user did not specify
find_program(CLANG_TIDY_BIN
        NAMES clang-tidy clang-tidy-12
        HINTS ${FALCONLINK_CLANG_SEARCH_PATH})

if ("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
    message(WARNING "FalconLink/main couldn't find clang-tidy.")
else ()
    # Output compile_commands.json
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    message(STATUS "FalconLink/main found clang-tidy at ${CLANG_TIDY_BIN}")
endif ()

# find_program(CLANG_APPLY_REPLACEMENTS_BIN
#         NAMES clang-apply-replacements clang-apply-replacements-12
#         HINTS ${BUSTUB_CLANG_SEARCH_PATH})

# if ("${CLANG_APPLY_REPLACEMENTS_BIN}" STREQUAL "CLANG_APPLY_REPLACEMENTS_BIN-NOTFOUND")
#     message(WARNING "FalconLink/main couldn't find clang-apply-replacements.")
# else ()
#     # Output compile_commands.json
#     set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
#     message(STATUS "FalconLink/main found clang-apply-replacements at ${CLANG_APPLY_REPLACEMENTS_BIN}")
# endif ()

# cpplint
find_program(CPPLINT_BIN
        NAMES cpplint cpplint.py
        HINTS ${FALCONLINK_BUILD_SUPPORT_DIR})
if ("${CPPLINT_BIN}" STREQUAL "CPPLINT_BIN-NOTFOUND")
    message(WARNING "FalconLink/main couldn't find cpplint.")
else ()
    message(STATUS "FalconLink/main found cpplint at ${CPPLINT_BIN}")
endif ()

######################################################################################################################
# COMPILER SETUP
######################################################################################################################

# TODO(catch22): add sanitizer
# if (NOT FALCONLINK_SANITIZER)
#     set(FALCONLINK_SANITIZER address)
# endif ()

message("Build mode: ${CMAKE_BUILD_TYPE}")
# message("${FALCONLINK_SANITIZER} sanitizer will be enabled in debug mode.")

# Compiler flags.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-unused-parameter -Wno-attributes") #TODO: remove
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")

# Output directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Includes.
SET(FALCONLINK_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/falconlink/include)
# set(FALCONLINK_TEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/test/include)  ## no test include right now
set(FALCONLINK_THIRD_PARTY_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/third_party
        )

include_directories(${FALCONLINK_SRC_INCLUDE_DIR} ${FALCONLINK_THIRD_PARTY_INCLUDE_DIR})
include_directories(BEFORE falconlink) # This is needed for gtest.

######################################################################################################################
# Other CMake modules
# MUST BE ADDED AFTER CONFIGURING COMPILER PARAMETERS
######################################################################################################################

add_subdirectory(falconlink)
add_subdirectory(test)
add_subdirectory(third_party)
add_subdirectory(examples)


# Build the http server
ADD_EXECUTABLE(http_server ${FALCONLINK_SRC_DIR}/http/http_server.cpp)
TARGET_LINK_LIBRARIES(http_server falconlink_net falconlink_http)
TARGET_COMPILE_OPTIONS(http_server PRIVATE ${CMAKE_COMPILER_FLAG})
TARGET_INCLUDE_DIRECTORIES(
        http_server
        PUBLIC ${FALCONLINK_SRC_INCLUDE_DIR}
)

######################################################################################################################
# MAKE TARGETS
######################################################################################################################

##########################################
# "make format"
# "make check-format"
##########################################

string(CONCAT FALCONLINK_FORMAT_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/falconlink,"
        "${CMAKE_CURRENT_SOURCE_DIR}/test,"
        "${CMAKE_CURRENT_SOURCE_DIR}/examples,"
        )

# Runs clang format and updates files in place.
add_custom_target(format ${FALCONLINK_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${FALCONLINK_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${FALCONLINK_FORMAT_DIRS}
        --fix
        --quiet
        )
    
# Runs clang format and exits with a non-zero exit code if any files need to be reformatted
add_custom_target(check-format ${FALCONLINK_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${FALCONLINK_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${FALCONLINK_FORMAT_DIRS}
        --quiet
        )

##########################################
# "make check-lint"
##########################################
file(GLOB_RECURSE FALCONLINK_LINT_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/falconlink/*.h*"
        "${CMAKE_CURRENT_SOURCE_DIR}/falconlink/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.h*"
        "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h*"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
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
        ${FALCONLINK_BUILD_SUPPORT_DIR}/run_clang_tidy.py                 # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN}                              # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR}                                            # using cmake's generated compile commands
        )
# add_dependencies(check-clang-tidy pine_shared)                    # needs gtest headers, compile_commands.json
