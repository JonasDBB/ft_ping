#include "ft_clib.h"

size_t ft_strlen(const char *s) {
    size_t ret;
    for (ret = 0; s[ret]; ++ret);
    return ret;
}

void *ft_memset(void *b, int c, size_t len) {
    unsigned char buf[MEMSET_TYPE_SIZE];
    for (unsigned int i = 0; i < MEMSET_TYPE_SIZE; ++i) {
        buf[i] = (unsigned char)c;
    }

    MEMSET_TYPE *created_type = (MEMSET_TYPE*)buf;
    MEMSET_TYPE *type_ptr = (MEMSET_TYPE*)b;
    for (; len > MEMSET_TYPE_SIZE; len -= MEMSET_TYPE_SIZE, ++type_ptr) {
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