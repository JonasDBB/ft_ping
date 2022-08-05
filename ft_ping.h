#ifndef FT_PING_FT_PING_H
#define FT_PING_FT_PING_H

#include <stdbool.h>
#include <sys/time.h>

#define SILENCE_END
#define PCKT_SIZE 64

enum exitcode {
    EXIT_OK = 0,
    EXIT_ERR = 1,
    EXIT_OTHER = 2
};

typedef struct str_str_pair_s {
    const char* first;
    const char* second;
} str_str_pair_t;


typedef struct s_options {
    bool verbose;
    char* hostname[10];
    unsigned int nr_of_hosts;
} options_t;

void end(const char* hostname);

void sigint_handler(int param);
void fatal_err(const char* err);
float get_time_since_in_ms(const struct timeval *first, const struct timeval *second);

// remove later
void send_sigint_to_self();
void print_os_name();


#endif //FT_PING_FT_PING_H
