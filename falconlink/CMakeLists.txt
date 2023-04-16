add_subdirectory(common)
add_subdirectory(net)
add_subdirectory(http)

add_library(falconlink STATIC ${ALL_OBJECT_FILES})

set(FALCONLINK_LIBS
        falconlink_common
        falconlink_net
        falconlink_http
        )


target_link_libraries(
        falconlink
        ${FALCONLINK_LIBS})

target_include_directories(
        falconlink PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
