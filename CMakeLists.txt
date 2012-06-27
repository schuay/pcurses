cmake_minimum_required (VERSION 2.8.0)
project (pcurses)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(pcurses_VERSION_MAJOR 1)
set(pcurses_VERSION_MINOR 5)
set(pcurses_APPLICATION_NAME pcurses)

configure_file(
    "${PROJECT_SOURCE_DIR}/globals.h.in"
    "${PROJECT_BINARY_DIR}/globals.h"
)

set(CURSES_NCURSES_REQUIRED TRUE)

find_package(Boost REQUIRED)
find_package(Curses REQUIRED)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -O2 -pedantic -fPIC -std=c++0x -D_FILE_OFFSET_BITS=64")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Werror -Wall -Wextra -pedantic -fPIC -std=c++0x -D_FILE_OFFSET_BITS=64")

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${Curses_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)
aux_source_directory(. sources)

add_executable(pcurses
    ${sources}
)

target_link_libraries(pcurses
    ${CURSES_LIBRARIES}
    alpm
)

install(TARGETS pcurses DESTINATION bin)
install(FILES pcurses.conf DESTINATION /etc)
