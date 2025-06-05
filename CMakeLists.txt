cmake_minimum_required(VERSION 3.31)
project(untitled013)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


add_executable(untitled013
        include/factory/unit_factory.h
        include/game/game.h
        include/game/game_board.h
        include/player/human_player.h
        include/player/player.h
        include/unit/unit.h
        include/unit/warrior.h
        include/unit/archer.h
        include/unit/mage.h
        include/constants.h
        src/factory/unit_factory.cpp
        src/game/game.cpp
        src/game/game_board.cpp
        src/player/human_player.cpp
        src/player/player.cpp
        src/unit/unit.cpp
        src/unit/warrior.cpp
        src/unit/archer.cpp
        src/unit/mage.cpp
        main.cpp
        src/constants.cpp
)
