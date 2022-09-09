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
