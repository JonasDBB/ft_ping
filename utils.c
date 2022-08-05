#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

#include "ft_ping.h"

void sigint_handler(int param) {
    (void)param;
    fprintf(stderr, "\nsigint handler\n");
    end("HOST_PLACEHOLDER");
}

void fatal_err(const char* err) {
    fprintf(stderr, "fatal error: %s\nerrno: %d\n", err, errno);
    perror("description");
    exit(EXIT_OTHER);
}

float get_time_since_in_ms(const struct timeval *first, const struct timeval *second) {
    return ((second->tv_sec - first->tv_sec) * 1000 + (second->tv_usec - first->tv_usec) / 1000.0f);
}

void send_sigint_to_self() {
    kill(getpid(), SIGINT);
}

#include <sys/utsname.h>
void print_os_name() {
    struct utsname uts;

    if (uname(&uts) == -1) {
        fatal_err("uname failed");
    }
    printf("system is %s\n", uts.sysname);
}
