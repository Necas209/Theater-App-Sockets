cmake_minimum_required(VERSION 3.28)
project(theater-app)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(JSON_ImplicitConversions OFF)

include_directories(src/lib)

add_executable(client
        src/lib/json.hpp
        src/lib/sockets.h
        src/lib/socket_error.h
        src/lib/extensions.h
        src/lib/handler.h
        src/lib/message.h
        src/lib/messages.h
        src/lib/show.h
        src/lib/theater.h
        src/client/client.h
        src/client/client.cpp
        src/client/main.cpp
)

add_executable(server
        src/lib/json.hpp
        src/lib/sockets.h
        src/lib/socket_error.h
        src/lib/extensions.h
        src/lib/message.h
        src/lib/messages.h
        src/lib/show.h
        src/lib/theater.h
        src/server/server_client.h
        src/server/server.h
        src/server/server.cpp
        src/server/main.cpp
)