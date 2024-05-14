cmake_minimum_required(VERSION 3.28)
project(theater-app)

set(CMAKE_CXX_STANDARD 23)

include_directories(src/client)
include_directories(src/server)
include_directories(src/lib)

add_executable(client
        src/client/main.cpp
        src/client/utils.cpp
        src/client/utils.h
        src/lib/json.hpp
        src/lib/message.cpp
        src/lib/message.h
        src/lib/show.cpp
        src/lib/show.h
        src/lib/theater.cpp
        src/lib/theater.h)

add_executable(server
        src/server/client.cpp
        src/server/client.h
        src/server/main.cpp
        src/server/utils.cpp
        src/server/utils.h
        src/lib/json.hpp
        src/lib/message.cpp
        src/lib/message.h
        src/lib/show.cpp
        src/lib/show.h
        src/lib/theater.cpp
        src/lib/theater.h)