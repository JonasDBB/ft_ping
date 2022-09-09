cmake_minimum_required(VERSION 3.16)
project(ft_ping C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_subdirectory(srcs)
add_subdirectory(clib EXCLUDE_FROM_ALL)
target_link_libraries(ft_ping ft_clib m)