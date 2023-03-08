cmake_minimum_required(VERSION 3.10)

include(GoogleTest)

file(GLOB_RECURSE FALCONLINK_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*/*test.cpp")

# #####################################################################################################################
# MAKE TARGETS
# #####################################################################################################################

# #########################################
# "make check-tests"
# #########################################
# add_custom_target(build-tests COMMAND ${CMAKE_CTEST_COMMAND} --show-only)
# add_custom_target(check-tests COMMAND ${CMAKE_CTEST_COMMAND} --verbose)
# add_custom_target(check-public-ci-tests COMMAND ${CMAKE_CTEST_COMMAND} --verbose -E SQLLogicTest)

# #########################################
# "make XYZ_test"
# #########################################
foreach (falconlink_test_source ${FALCONLINK_TEST_SOURCES})
    # Create a human readable name.
    get_filename_component(falconlin_test_filename ${falconlink_test_source} NAME)
    string(REPLACE ".cpp" "" falconlin_test_name ${falconlin_test_filename})

    # Add the test target separately and as part of "make check-tests".
    add_executable(${falconlin_test_name} EXCLUDE_FROM_ALL ${falconlink_test_source})
    add_dependencies(build-tests ${falconlin_test_name})
    add_dependencies(check-tests ${falconlin_test_name})

    gtest_discover_tests(${falconlin_test_name}
            EXTRA_ARGS
            --gtest_color=auto
            --gtest_output=xml:${CMAKE_BINARY_DIR}/test/${falconlin_test_name}.xml
            --gtest_catch_exceptions=0
            DISCOVERY_TIMEOUT 120
            PROPERTIES
            TIMEOUT 120
            )

    target_link_libraries(${falconlin_test_name} falconlink gtest gmock_main)

    # Set test target properties and dependencies.
    set_target_properties(${falconlin_test_name}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
            COMMAND ${falconlin_test_name}
            )
endforeach ()

# set(BUSTUB_SLT_SOURCES
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.01-seqscan.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.02-insert.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.03-delete.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.04-index-scan.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.05-empty-table.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.06-simple-agg.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.07-group-agg-1.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.08-group-agg-2.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.09-simple-join.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.10-multi-way-join.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.11-repeat-execute.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.12-nested-index-join.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.13-sort-limit.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.14-topn.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.15-integration-1.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.16-integration-2.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.leaderboard-q1.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.leaderboard-q2.slt"
#         "${PROJECT_SOURCE_DIR}/test/sql/p3.leaderboard-q3.slt"
#         )

# add_custom_target(test-p3 ${CMAKE_CTEST_COMMAND} -R SQLLogicTest)

# # #########################################
# # "make XYZ_test"
# # #########################################
# foreach (falconlink_test_source ${BUSTUB_SLT_SOURCES})
#     # Create a human readable name.
#     get_filename_component(falconlin_test_filename ${falconlink_test_source} NAME)
#     string(REPLACE ".slt" "" bustub_filename_wo_suffix "${falconlin_test_filename}")
#     string(REPLACE ".slt" "" falconlin_test_name "SQLLogicTest.${bustub_filename_wo_suffix}")
#     add_test(NAME ${falconlin_test_name} COMMAND "${CMAKE_BINARY_DIR}/bin/bustub-sqllogictest" ${falconlink_test_source} --verbose -d --in-memory)
#     add_custom_target(${bustub_filename_wo_suffix}_test COMMAND "${CMAKE_BINARY_DIR}/bin/bustub-sqllogictest" "${falconlink_test_source}" --verbose -d --in-memory)
#     add_dependencies(${bustub_filename_wo_suffix}_test sqllogictest)
# endforeach ()

# add_dependencies(test-p3 sqllogictest)

# # Must build sqllogictest before checking tests
# add_dependencies(build-tests sqllogictest)
# add_dependencies(check-tests sqllogictest)