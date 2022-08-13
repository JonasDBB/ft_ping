#include "ft_clib.h"
#include <stdio.h>
#include <stdarg.h>

size_t ft_strlen(const char* s) {
    size_t ret;
    for (ret = 0; s[ret]; ++ret);
    return ret;
}

void* ft_memset(void* b, int c, size_t len) {
    unsigned char buf[MEMSET_TYPE_SIZE];
    for (unsigned int i = 0; i < MEMSET_TYPE_SIZE; ++i) {
        buf[i] = (unsigned char)c;
    }

    MEMSET_TYPE* created_type = (MEMSET_TYPE*)buf;
    MEMSET_TYPE* type_ptr = (MEMSET_TYPE*)b;
    for (; len > MEMSET_TYPE_SIZE; len -= MEMSET_TYPE_SIZE, ++type_ptr) {
        *type_ptr = *created_type;
    }

    unsigned char* char_ptr = (unsigned char*)type_ptr;
    for (; len; --len, ++char_ptr) {
        *char_ptr = (unsigned char)c;
    }
    return b;
}

void ft_bzero(void* s, size_t n) {
    ft_memset(s, 0, n);
}

char* ft_strchr(char* s, int c) {
    for (; *s && *s != c; ++s);
    return *s == c ? s : NULL;
}

char* ft_strrchr(char* s, int c) {
    char *t = s;
    for (; *t; ++t);
    for (; t != s && *t != c; --t);
    return *t == c ? t : NULL;
}

void log_func(const char* file, const char* func, int line, const char* fmt, ...) {
    fprintf(stderr, "%s:%d: %s(): [", file, line, func);

    va_list varlist;
    va_start(varlist, fmt);
    vfprintf(stderr, fmt, varlist);
    va_end(varlist);

    fprintf(stderr, "]\n");
}