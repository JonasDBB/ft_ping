#include "ft_clib.h"

size_t ft_strlen(const char *s) {
    size_t ret;
    for (ret = 0; s[ret]; ++ret);
    return ret;
}

#define TYPE long long
#define TYPE_SIZE sizeof(TYPE)
void *ft_memset(void *b, int c, size_t len) {
    unsigned char buf[TYPE_SIZE];
    for (unsigned int i = 0; i < TYPE_SIZE; ++i) {
        buf[i] = (unsigned char)c;
    }

    TYPE *created_type = (TYPE*)buf;
    TYPE *type_ptr = (TYPE*)b;
    for (; len > TYPE_SIZE; len -= TYPE_SIZE, ++type_ptr) {
        *type_ptr = *created_type;
    }

    unsigned char *char_ptr = (unsigned char*)type_ptr;
    for (; len; --len, ++char_ptr) {
        *char_ptr = (unsigned char)c;
    }

    return b;
}

void ft_bzero(void *s, size_t n) {
    ft_memset(s, 0, n);
}