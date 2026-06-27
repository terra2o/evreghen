cmake_minimum_required(VERSION 3.28)

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

if(CMAKE_C_COMPILER)
endif()

project(
    evreghen
    VERSION 1.0.0
    DESCRIPTION "A Cross-Platform Game Engine and Build System for Terminal Games"
    HOMEPAGE_URL "https://github.com/jayshozie/evreghen" # @REMINDER: the project will be moved to a separate GitHub account called evreghen at some point
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(EVREGHEN_NO_GUI "If this flag is enabled, the evreghen game engine will be built without the GUI. It will only act as the build system. Without this flag set to ON, evreghen will be built with the GUI, and will act as a fully-fledged game engine.")

add_subdirectory(engine)
if(EVREGHEN_NO_GUI)
    message("[EVREGHEN] [INFO] evreghen will be built WITHOUT the GUI!")
else()
    message("[EVREGHEN] [INFO] evreghen will be built WITH the GUI.")
    add_subdirectory(gui)
endif()
add_subdirectory(cli)
