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

# Detect the operating system
MESSAGE("Compiling on the operating system of ${CMAKE_SYSTEM_NAME}")
IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    ADD_DEFINITIONS(-DOS_LINUX)
ELSE()
    MESSAGE(FATAL_ERROR "Your operating system ${CMAKE_SYSTEM_NAME} is not supported.")
ENDIF()

######################################################################################################################
# Include
######################################################################################################################
SET(FALCONLINK_SRC_DIR ${PROJECT_SOURCE_DIR}/falconlink)
SET(FALCONLINK_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/falconlink/include)
SET(FALCONLINK_EXAMPLE_DIR ${PROJECT_SOURCE_DIR}/examples)

######################################################################################################################
# Build
######################################################################################################################
# SET(CMAKE_COMPILER_FLAG -O3 -Wall -Wextra -pedantic -Werror)
SET(CMAKE_COMPILER_FLAG -O3 -Wall -Wextra)

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