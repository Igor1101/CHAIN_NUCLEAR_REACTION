cmake_minimum_required (VERSION 2.6)
project (NUCLEA_DECAY)
set(CMAKE_C_FLAGS "-Wall -g -O4 ")
find_package(SDL2 REQUIRED)
set(ALL_LIBS
m
SDL2main
SDL2
)

include_directories( . )
add_executable(DECAY main.c)

target_link_libraries(DECAY ${ALL_LIBS})


