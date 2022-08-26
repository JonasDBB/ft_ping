#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>

#include "ft_ping.h"
#include "clib/ft_clib.h"

extern bool active;
extern bool send_next_msg;

void sigint_handler(int i) {
    (void)i;
    active = false;
}

void sigalrm_handler(int i) {
    (void)i;
    signal(SIGALRM, sigalrm_handler);
    send_next_msg = true;
}

void fatal_err(const char* err) {
    fprintf(stderr, "fatal error: %s\nerrno %d: %s\n", err, errno, errno_name(errno));
    perror("description");
    exit(EXIT_OTHER);
}

float get_time_since_in_ms(const struct timeval* first, const struct timeval* second) {
    return ((second->tv_sec - first->tv_sec) * 1000 + (second->tv_usec - first->tv_usec) / 1000.0f);
}

#include <sys/utsname.h>
void print_os_name() {
    struct utsname uts;

    if (uname(&uts) == -1) {
        fatal_err("uname failed");
    }
    printf("system is %s\n", uts.sysname);
}
