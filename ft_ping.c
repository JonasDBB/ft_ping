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

static str_str_pair_t flag_map[] = {
        {"<destination>", "dns name or ip address"},
        {"-h", "help"},
        {"-v", "verbose"}
};

bool active = true;
bool send_next_msg = true;

void help(char invalid_flag) {
    if (invalid_flag) {
        fprintf(stderr, "ft_ping: invalid option -- '%c'\n", invalid_flag);
    }
    fprintf(stderr, "\nUsage\n"
           "  ft_ping [options] <destination>\n"
           "Options:\n");
    const unsigned int len = sizeof(flag_map) > 0 ? sizeof(flag_map) / sizeof flag_map[0] : 0;
    for (unsigned int i = 0; i < len; ++i) {
        fprintf(stderr, "  %-15s%s\n", flag_map[i].first, flag_map[i].second);
    }

    exit(EXIT_OTHER);
}

options_t parse_args(int ac, char** av) {
    options_t ret_val;
    ft_bzero(&ret_val, sizeof(ret_val));
    for (int i = 1; i < ac; ++i) {
        if (av[i][0] == '-') {
            for (unsigned int j = 1; j < ft_strlen(av[i]); ++j) {
                switch (av[i][j]) {
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
    printf("result here later, exiting cleanly...\n");
#endif
    exit(EXIT_OK);
}

int main(int ac, char** av) {
    print_os_name();

    signal(SIGINT, sigint_handler);
    signal(SIGALRM, sigalrm_handler);

    options_t opts = parse_args(ac, av);
    struct addrinfo* ai = find_addr_info(&opts);
    const int sockfd = socket_setup();
    const static_info_t static_info = set_ip_info(ai);

    unsigned char buffer[PCKT_SIZE + sizeof(struct iphdr)];
    ft_bzero(buffer, PCKT_SIZE + sizeof(struct iphdr));
    struct icmphdr* icmp;
    icmp = (struct icmphdr*)buffer;

    received_msg_t received_msg;

    msg_init(icmp, &received_msg);

    while (active) {
        if (send_next_msg) {
            LOG("send_next_msg");
            send_next_msg = false;
            alarm(2);
            ping_loop(sockfd, ai, icmp, &received_msg, &static_info);
            LOG("done");
        }
    }

    freeaddrinfo(ai);
    close(sockfd);
    end(av[0]);
    return 0;
}
