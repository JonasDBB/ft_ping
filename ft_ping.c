#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <iso646.h>
#include <errno.h>

#include "ft_ping.h"

void help(char invalid_flag) {
    if (invalid_flag) {
        printf("ft_ping: invalid option -- '%c'\n", invalid_flag);
    }
    printf("\nUsage\n"
           "  ft_ping [options] <destination>\n"
           "Options:\n");
    const unsigned int len = sizeof(flag_map) > 0 ? sizeof(flag_map) / sizeof flag_map[0] : 0;
    for (unsigned int i = 0; i < len; ++i) {
        printf("  %-15s%s\n", flag_map[i].flag, flag_map[i].details);
    }

    exit(EXIT_OTHER);
}

options_t parse_args(int ac, char **av) {
    options_t ret_val;
    bzero(&ret_val, sizeof(ret_val));
    for (int i = 1; i < ac; ++i) {
        if (av[i][0] == '-') {
            for (unsigned int j = 1; j < strlen(av[i]); ++j) {
                switch (av[i][j])
                {
                    case 'h':
                        help(0);
                        break;
                    case 'v':
                        ret_val.verbose = true;
                        break;
                    default:
                        help(av[i][j]);
                }
            }
        } else {
            ret_val.hostname[ret_val.nr_of_hosts++] = av[i];
        }
    }
    if (ret_val.nr_of_hosts == 0) {
        printf("ft_ping: usage error: Destination address required\n");
        exit(EXIT_ERR);
    }
    if (ret_val.nr_of_hosts > 10) {
        help(0);
    }
    return ret_val;
}

void end(const char* hostname) {
    printf("--- %s ft_ping statistics ---\n", hostname);
    printf("%d packets transmitted, %d received, %d%% packet loss, time %fms\n", 0, 0, 0, 0.f);
    printf("rtt min/avg/max/mdev = %f/%f/%f/%f ms\n", 0.f, 0.f, 0.f, 0.f);


    printf("result here later, exiting cleanly");
    exit(EXIT_OK);
}

void sigint_handler(int param) {
    (void)param;
    printf("sigint handler");
    end("HOST_PLACEHOLDER");
}

void fatal_err(const char* err) {
    dprintf(2, "fatal error: %s\n", err);
    perror("error: ");
    exit(EXIT_OTHER);
}

void send_sigint_to_self() {
    kill(getpid(), SIGINT);
}

float get_time_since_in_ms(const struct timeval *first, const struct timeval *second) {
    return ((second->tv_sec - first->tv_sec) * 1000 + (second->tv_usec - first->tv_usec) / 1000.0f);
}

int main(int ac, char **av)
{
    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    signal(SIGINT, sigint_handler);
    options_t opts = parse_args(ac, av);

    struct addrinfo hints;
    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;

    struct addrinfo* ai = NULL;
    if (getaddrinfo(opts.hostname[0], NULL, &hints, &ai) != 0) {
        freeaddrinfo(ai);
        fatal_err("getaddrinfo fail");
    }

    struct addrinfo* list;
    for (list = ai; list != NULL; list = list->ai_next) {
        char buf[INET_ADDRSTRLEN];
        struct sockaddr_in* s = (struct sockaddr_in*)list->ai_addr;
        printf("ip is %s\n", inet_ntop(AF_INET, &s->sin_addr, buf, sizeof(buf)));
    }
    fflush(stdout);
    freeaddrinfo(ai);
    end(av[0]);
    return 0;
}
