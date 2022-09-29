#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>

#include "ft_ping.h"
#include "ft_clib.h"

extern loop_t loop;

void sigint_handler(int i) {
    (void)i;
    loop.active = false;
}

void sigalrm_handler(int i) {
    (void)i;
    signal(SIGALRM, sigalrm_handler);
    loop.send_next_msg = true;
}

void fatal_err(const char* err) {
    fprintf(stderr, "fatal error: %s\nerrno %d: %s\n", err, errno, errno_name(errno));
    exit(EXIT_OTHER);
}

double get_time_since_in_ms(const struct timeval* first, const struct timeval* second) {
    return ((double)(second->tv_sec - first->tv_sec) * 1000 + (double)(second->tv_usec - first->tv_usec) / 1000.0f);
}

bool is_valid_ipv4_address(const char* s) {
    int n_segment = 0;
    int n_char = 0;
    int segm_sum = 0;

    if (!s) {
        return false;
    }

    while (*s) {
        if (*s == '.') {
            if (n_char == 0) { // '.' as first cahr
                return false;
            }
            if (++n_segment == 4) { // more than 4 segments
                return false;
            }
            n_char = 0;
            segm_sum = 0;
            ++s;
            continue;
        }
        if (ft_isnum(*s) == false) { // not digit
            return false;
        }
        segm_sum = segm_sum * 10 + (*s - '0');
        if (segm_sum > 255) { // n_segment is higher than 255
            return false;
        }
        ++n_char;
        ++s;
    }
    if (n_segment != 3 || n_char == 0) { // not 4 segments or empty string
        return false;
    }
    return true;
}