cmake_minimum_required(VERSION 3.30)
project(Hex_Sem_project)
include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(Hex_Sem_project
        notification.cpp
        HexNode.cpp
        GameState.cpp
        TurnHandler.cpp
        main.cpp
        fileHandler.cpp
        GameList.cpp
        GameList.h
        buttonGen.cpp
)
target_link_libraries(Hex_Sem_project PRIVATE SFML::Graphics)