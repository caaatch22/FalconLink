set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # don't override our compiler/linker options when building gtest
add_subdirectory(googletest)

# 'make benchmark'
# will run http_server
SET(WEBBENCH_DIR ${PROJECT_SOURCE_DIR}/third_party/webbench)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET(CONCURRENCY 10500)  # concurrent client number
    SET(DURATION 5)  # stress test duration
ENDIF()

ADD_CUSTOM_TARGET(benchmark
        WORKING_DIRECTORY ${WEBBENCH_DIR}
        COMMAND sh ./benchmark.sh ${CONCURRENCY} ${DURATION}
        DEPENDS http_server
        )
