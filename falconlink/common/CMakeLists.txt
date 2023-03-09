add_library(
  falconlink_common
  OBJECT
  buffer.cpp
  thread_pool.cpp)


find_package(Threads REQUIRED)

target_link_libraries(falconlink_common PUBLIC Threads::Threads)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:falconlink_common>
    PARENT_SCOPE)
