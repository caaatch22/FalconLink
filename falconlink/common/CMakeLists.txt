add_library(
  falconlink_common
  OBJECT
  buffer.cpp
  thread_pool.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:falconlink_common>
    PARENT_SCOPE)
