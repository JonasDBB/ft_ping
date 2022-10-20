cmake_minimum_required(VERSION 3.16)
project(ft_ping C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})


add_executable(ft_ping)

add_subdirectory(srcs)

include_directories(
		${CMAKE_SOURCE_DIR}/incl
		${CMAKE_SOURCE_DIR}/clib/ft_clib EXCLUDE_FROM_ALL
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


add_subdirectory(clib EXCLUDE_FROM_ALL)
target_link_libraries(ft_ping ft_clib m)