#include "ft_clib.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <stdbool.h>

static const char* const errnames[] = {
    [0] = 0,
#ifdef EPERM
    [EPERM] = "EPERM",
#endif
#ifdef ENOENT
        [ENOENT] = "ENOENT",
#endif
#ifdef ESRCH
        [ESRCH] = "ESRCH",
#endif
#ifdef EINTR
        [EINTR] = "EINTR",
#endif
#ifdef EIO
        [EIO] = "EIO",
#endif
#ifdef ENXIO
        [ENXIO] = "ENXIO",
#endif
#ifdef E2BIG
        [E2BIG] = "E2BIG",
#endif
#ifdef ECHILD
        [ECHILD] = "ECHILD",
#endif
#ifdef EAGAIN
        [EAGAIN] = "EAGAIN",
#endif
#ifdef ENOMEM
        [ENOMEM] = "ENOMEM",
#endif
#ifdef EACCES
        [EACCES] = "EACCES",
#endif
#ifdef EFAULT
        [EFAULT] = "EFAULT",
#endif
#ifdef ENOTBLK
        [ENOTBLK] = "ENOTBLK",
#endif
#ifdef EBUSY
        [EBUSY] = "EBUSY",
#endif
#ifdef EEXIST
        [EEXIST] = "EEXIST",
#endif
#ifdef EXDEV
        [EXDEV] = "EXDEV",
#endif
#ifdef ENODEV
        [ENODEV] = "ENODEV",
#endif
#ifdef ENOTDIR
        [ENOTDIR] = "ENOTDIR",
#endif
#ifdef EISDIR
        [EISDIR] = "EISDIR",
#endif
#ifdef EINVAL
        [EINVAL] = "EINVAL",
#endif
#ifdef ENFILE
        [ENFILE] = "ENFILE",
#endif
#ifdef EMFILE
        [EMFILE] = "EMFILE",
#endif
#ifdef ENOTTY
        [ENOTTY] = "ENOTTY",
#endif
#ifdef ETXTBSY
        [ETXTBSY] = "ETXTBSY",
#endif
#ifdef EFBIG
        [EFBIG] = "EFBIG",
#endif
#ifdef ENOSPC
        [ENOSPC] = "ENOSPC",
#endif
#ifdef ESPIPE
        [ESPIPE] = "ESPIPE",
#endif
#ifdef EROFS
        [EROFS] = "EROFS",
#endif
#ifdef EMLINK
        [EMLINK] = "EMLINK",
#endif
#ifdef EPIPE
        [EPIPE] = "EPIPE",
#endif
#ifdef EDOM
        [EDOM] = "EDOM",
#endif
#ifdef ERANGE
        [ERANGE] = "ERANGE",
#endif
};

const char* errno_name(int e) {
    return errnames[e];
}

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

bool ft_isnum(char c) {
    return c >= '0' && c <= '9';
}

bool ft_isspace(char c) {
    return c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r' || c == ' ';
}

void log_func(const char* file, const char* func, int line, const char* fmt, ...) {
    char buf[MAX_LOG_SIZE];
    va_list varlist;
    va_start(varlist, fmt);
    vsnprintf(buf, MAX_LOG_SIZE, fmt, varlist);
    va_end(varlist);

    struct timeval t;
    gettimeofday(&t, NULL);
    struct tm *tm = localtime(&t.tv_sec);

    fprintf(stderr, "%02d:%02d:%02d.%03ld| %s:%d: %s(): [%s]\n",
            tm->tm_hour, tm->tm_min, tm->tm_sec, (t.tv_usec / 1000), file, line, func, buf);
}