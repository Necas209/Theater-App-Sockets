cmake_minimum_required(VERSION 3.22)
project(TheaterApp)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)
include_directories(Client)
include_directories(Server)
include_directories(TheaterLibrary)

add_executable(ClientApp
        Client/main.cpp
        Client/utils.cpp
        Client/utils.h
        TheaterLibrary/json.hpp
        TheaterLibrary/message.cpp
        TheaterLibrary/message.h
        TheaterLibrary/show.cpp
        TheaterLibrary/show.h
        TheaterLibrary/theater.cpp
        TheaterLibrary/theater.h)

add_executable(Server
        Server/client.cpp
        Server/client.h
        Server/main.cpp
        Server/utils.cpp
        Server/utils.h
        TheaterLibrary/json.hpp
        TheaterLibrary/message.cpp
        TheaterLibrary/message.h
        TheaterLibrary/show.cpp
        TheaterLibrary/show.h
        TheaterLibrary/theater.cpp
        TheaterLibrary/theater.h)