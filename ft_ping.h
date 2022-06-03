#ifndef FT_PING_FT_PING_H
#define FT_PING_FT_PING_H

#include <stdbool.h>

enum exitcode {
    EXIT_OK = 0,
    EXIT_ERR = 1,
    EXIT_OTHER = 2
};

typedef struct flag_s {
    const char* flag;
    const char* details;
} flag_t;

static flag_t flag_map[] = {
        {"<destination>", "dns name or ip address"},
        {"-h", "help"},
        {"-v", "verabose"}
};

typedef struct s_options {
    bool verbose;
    char* hostname[10];
    unsigned int nr_of_hosts;
} options_t;


#endif //FT_PING_FT_PING_H
