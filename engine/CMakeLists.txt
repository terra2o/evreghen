# evreghen - A Cross-Platform Game Engine and Build System for Terminal Games
# Copyright (C) 2026  terra2o <terra2o@protonmail.com>
# Copyright (C) 2026  Emir Baha Yıldırım <jayshozie@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

if(WIN32)
    find_package(PDCurses QUIET)
    if(NOT PDCURSES_FOUND)
        message(STATUS "PDCurses not found via CMake package, looking for PDCurses library")
        find_library(PDCURSES_LIBRARY NAMES pdcurses curses)
        find_path(PDCURSES_INCLUDE_DIR NAMES curses.h)
        set(CURSES_LIBRARIES ${PDCURSES_LIBRARY})
        set(CURSES_INCLUDE_DIRS ${PDCURSES_INCLUDE_DIR})
    endif()
else()
    find_package(Curses REQUIRED)
endif()

add_library(engine STATIC
    src/tile_registry.cpp
    src/entity.cpp
    src/player.cpp
    src/npc.cpp
    src/enemy.cpp
    src/map.cpp
    src/map_loader.cpp
    src/level_manager.cpp
    src/renderer.cpp
    src/resource.cpp
    src/node.cpp
)

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CURSES_INCLUDE_DIRS}
)

target_link_libraries(engine PUBLIC
    nlohmann_json::nlohmann_json
    ${CURSES_LIBRARIES}
)
