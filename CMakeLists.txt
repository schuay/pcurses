cmake_minimum_required (VERSION 3.10)
project (pcurses)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(pcurses_VERSION 4)
set(pcurses_APPLICATION_NAME pcurses)

configure_file(
    "${PROJECT_SOURCE_DIR}/src/globals.h.in"
    "${PROJECT_BINARY_DIR}/src/globals.h"
)

set(CURSES_NCURSES_REQUIRED TRUE)

find_package(Boost REQUIRED)
find_package(Curses REQUIRED)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(
        -Wall -Wextra -Werror -O2 -pedantic -fPIC -std=c++17
        -D_FILE_OFFSET_BITS=64
    )
endif(CMAKE_COMPILER_IS_GNUCXX)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${Curses_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}/src
)
aux_source_directory(src/ sources)

add_executable(pcurses
    ${sources}
)

target_link_libraries(pcurses
    ${CURSES_LIBRARIES}
    alpm
)

install(TARGETS pcurses DESTINATION bin)
install(FILES pcurses.conf DESTINATION /etc)
