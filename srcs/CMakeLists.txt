include_directories(
		${CMAKE_SOURCE_DIR}/incl
		${CMAKE_SOURCE_DIR}/clib EXCLUDE_FROM_ALL
)

add_executable(ft_ping
		ft_ping.c
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

