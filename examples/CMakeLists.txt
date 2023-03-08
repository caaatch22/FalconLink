SET(FALCONLINK_EXAMPLE_DIR ${PROJECT_SOURCE_DIR}/examples)

# Build the echo server
ADD_EXECUTABLE(echo_server ${FALCONLINK_EXAMPLE_DIR}/echo/echo_server.cpp)
TARGET_LINK_LIBRARIES(echo_server falconlink_net falconlink_common)
TARGET_COMPILE_OPTIONS(echo_server PRIVATE ${CMAKE_COMPILER_FLAG})
TARGET_INCLUDE_DIRECTORIES(
        echo_server
        PUBLIC ${FALCONLINK_SRC_INCLUDE_DIR}
)

# Build the echo client
ADD_EXECUTABLE(echo_client ${FALCONLINK_EXAMPLE_DIR}/echo/echo_client.cpp)
TARGET_LINK_LIBRARIES(echo_client falconlink_net falconlink_common)
TARGET_INCLUDE_DIRECTORIES(
        echo_client
        PUBLIC ${FALCONLINK_SRC_INCLUDE_DIR}
)

# Build the http server
ADD_EXECUTABLE(http_server ${FALCONLINK_EXAMPLE_DIR}/http_server/http_server.cpp)
TARGET_LINK_LIBRARIES(http_server falconlink_net falconlink_common falconlink_http)
TARGET_COMPILE_OPTIONS(http_server PRIVATE ${CMAKE_COMPILER_FLAG})
TARGET_INCLUDE_DIRECTORIES(
        http_server
        PUBLIC ${FALCONLINK_SRC_INCLUDE_DIR}
)