add_library(
        falconlink_net
        OBJECT
        acceptor.cpp
        cache.cpp
        connection.cpp
        event_loop.cpp
        inet_addr.cpp
        poller.cpp
        socket.cpp)

set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:falconlink_net>
        PARENT_SCOPE)
