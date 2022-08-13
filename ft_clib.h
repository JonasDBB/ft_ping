#ifndef FT_PING_FT_CLIB_H
#define FT_PING_FT_CLIB_H

#include <stddef.h>

#define MEMSET_TYPE long long
#define MEMSET_TYPE_SIZE sizeof(MEMSET_TYPE)

#define __FILENAME__ (ft_strrchr(__FILE__, '/') ? ft_strrchr(__FILE__, '/') + 1 : __FILE__)

void log_func(const char* file, const char* func, int line, const char* fmt, ...);
#define LOG(fmt, ...) { \
    log_func(__FILENAME__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__); \
}

char* ft_strchr(char* s, int c);
char* ft_strrchr(char* s, int c);
size_t ft_strlen(const char* s);
void* ft_memset(void* b, int c, size_t len);
void ft_bzero(void* s, size_t n);

#endif //FT_PING_FT_CLIB_H
