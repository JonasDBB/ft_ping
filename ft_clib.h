#ifndef FT_PING_FT_CLIB_H
#define FT_PING_FT_CLIB_H

#include <stddef.h>

#define MEMSET_TYPE long long
#define MEMSET_TYPE_SIZE sizeof(MEMSET_TYPE)

size_t ft_strlen(const char *s);
void *ft_memset(void *b, int c, size_t len);
void ft_bzero(void *s, size_t n);

#endif //FT_PING_FT_CLIB_H
