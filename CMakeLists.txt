cmake_minimum_required(VERSION 3.16)
project(ft_ping C)

set(CMAKE_C_STANDARD 99)

add_executable(ft_ping
		ft_ping.c
		ft_ping.h
		utils.c
		ping_loop.c
		setup.c
		)

target_compile_options(ft_ping PUBLIC
		-Wall
		-Werror
		-Wextra
		-g3
		-fsanitize=address
		)

target_link_options(ft_ping PUBLIC
		-fsanitize=address
		)

add_subdirectory(clib)
target_link_libraries(ft_ping ft_clib m)