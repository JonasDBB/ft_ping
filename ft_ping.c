#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <iso646.h>
#include <netinet/ip_icmp.h>

#include "ft_clib.h"
#include "ft_ping.h"

static flag_t flag_map[] = {
        {"<destination>", "dns name or ip address"},
        {"-h", "help"},
        {"-v", "verbose"}
};

void help(char invalid_flag) {
    if (invalid_flag) {
        fprintf(stderr, "ft_ping: invalid option -- '%c'\n", invalid_flag);
    }
    fprintf(stderr, "\nUsage\n"
           "  ft_ping [options] <destination>\n"
           "Options:\n");
    const unsigned int len = sizeof(flag_map) > 0 ? sizeof(flag_map) / sizeof flag_map[0] : 0;
    for (unsigned int i = 0; i < len; ++i) {
        fprintf(stderr, "  %-15s%s\n", flag_map[i].flag, flag_map[i].details);
    }

    exit(EXIT_OTHER);
}

options_t parse_args(int ac, char **av) {
    options_t ret_val;
    ft_bzero(&ret_val, sizeof(ret_val));
    for (int i = 1; i < ac; ++i) {
        if (av[i][0] == '-') {
            for (unsigned int j = 1; j < ft_strlen(av[i]); ++j) {
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
            if (ret_val.nr_of_hosts >= 10) {
                help(0);
            }
            ret_val.hostname[ret_val.nr_of_hosts++] = av[i];
        }
    }
    if (ret_val.nr_of_hosts == 0) {
        printf("ft_ping: usage error: Destination address required\n");
        exit(EXIT_ERR);
    }
    return ret_val;
}

void end(const char* hostname) {
#ifndef SILENCE_END
    printf("--- %s ft_ping statistics ---\n", hostname);
    printf("%d packets transmitted, %d received, %d%% packet loss, time %fms\n", 0, 0, 0, 0.f);
    printf("rtt min/avg/max/mdev = %f/%f/%f/%f ms\n", 0.f, 0.f, 0.f, 0.f);
#else
    (void)hostname;
    printf("result here later, exiting cleanly");
#endif
    exit(EXIT_OK);
}

int main(int ac, char **av)
{
    print_os_name();

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    signal(SIGINT, sigint_handler);
    options_t opts = parse_args(ac, av);

    struct addrinfo hints;
    ft_bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;

    struct addrinfo* ai = NULL;
    int ret = getaddrinfo(opts.hostname[0], NULL, &hints, &ai);
    if (ret != 0) {
        freeaddrinfo(ai);
        if (ret == EAI_NONAME) {
            fprintf(stderr, "ft_ping: %s: Temporary failure in name resolution\n", opts.hostname[0]);
            exit(EXIT_OTHER);
        } else {
            fatal_err("getaddrinfo fail");
        }
    }

    struct addrinfo* list;
    for (list = ai; list != NULL; list = list->ai_next) {
        char buf[INET_ADDRSTRLEN];
        struct sockaddr_in* s = (struct sockaddr_in*)list->ai_addr;
        printf("ip is %s\n", inet_ntop(AF_INET, &s->sin_addr, buf, sizeof(buf)));
    }

    int sck = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sck < 0) {
        fatal_err("socket failure");
    }

    unsigned char buf[1000];
    ft_bzero(buf, 1000);
    struct icmphdr *icmp;

    icmp = (struct icmphdr*)buf;
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->checksum = 0;
    icmp->un.echo.id = getpid();
    static int counter = 0;
    icmp->un.echo.sequence = counter++;
    // compute checksum

    ssize_t sendret = sendto(sck, icmp, sizeof(icmp), 0, (struct sockaddr*)list->ai_addr,
                         sizeof(list->ai_addr));
    if (sendret < 0) {
        fatal_err("sendto error");
    }


    fflush(stdout);
    freeaddrinfo(ai);
    end(av[0]);
    return 0;
}
