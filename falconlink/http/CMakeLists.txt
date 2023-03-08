add_library(
  falconlink_http
  OBJECT
  cgier.cpp
  request.cpp
  response.cpp
  http_server.cpp
)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:falconlink_http>
  PARENT_SCOPE)
