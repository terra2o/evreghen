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

add_subdirectory(./engine)
add_subdirectory(./cli)
add_subdirectory(./gui)
