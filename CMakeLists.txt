# Copyright 2020-2021 Kevin Backhouse.
#
# This file is part of DBusParseDemo.
#
# DBusParseDemo is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DBusParseDemo is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with DBusParseDemo.  If not, see <https://www.gnu.org/licenses/>.


cmake_minimum_required(VERSION 3.10)

enable_testing()

# set the project name
project(DBusParseDemo VERSION 1.0.0 DESCRIPTION "Demos of using the DBusParse library")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(EPollLoop_DIR "${CMAKE_SOURCE_DIR}/EPollLoop")
set(DBusParse_DIR "${CMAKE_SOURCE_DIR}/DBusParse")

option(USE_SANITIZERS "Enable ASAN and UBSAN" OFF)

add_compile_options(-Wall -Wextra -pedantic -Werror)

if (USE_SANITIZERS)
  set(SANITIZER_FLAGS "-fsanitize=address,undefined")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZER_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZER_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${SANITIZER_FLAGS}")
endif()

add_subdirectory(DBusParse)
add_subdirectory(EPollLoop)
add_subdirectory(EPollLoopDBusHandler)

add_executable(introspect introspect.cpp)
target_link_libraries(introspect PUBLIC DBusParse DBusParseUtils)
target_include_directories(
        introspect PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/DBusParse/include/DBusParse>)

add_executable(introspect_async introspect_async.cpp)
target_link_libraries(introspect_async PUBLIC DBusParse DBusParseUtils EPollLoop EPollLoopDBusHandler)
target_include_directories(
        introspect_async PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/DBusParse/include/DBusParse>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/EPollLoop/include/EPollLoop>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/EPollLoopDBusHandler/include/EPollLoopDBusHandler>)
