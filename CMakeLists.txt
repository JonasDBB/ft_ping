cmake_minimum_required(VERSION 3.18)
project(ft_ping C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -g3 -fsanitize=address")
set(CMAKE_LINKER_FLAGS)

add_executable(ft_ping ft_ping.c ft_ping.h ft_clib.c ft_clib.h utils.c)
