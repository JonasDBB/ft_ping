#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <signal.h>
#include <netinet/ip_icmp.h>
#include <math.h>

#include "ft_clib.h"
#include "ft_ping.h"

static const str_str_pair_t flag_map[] = {
        {"<destination>", "dns name or ip address"},
        {"-a",            "use audible ping"},
        {"-h",            "help"},
        {"-i <interval>", "seconds between sending each packet"},
        {"-v",            "verbose"}
};

loop_t loop = {true, true};

static void help(char invalid_flag) {
    if (invalid_flag == 'i') {
        fprintf(stderr, "ft_ping: option requires an argument -- '%c'\n", invalid_flag);
    } else if (invalid_flag) {
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

static long get_interval(char* s) {
    for (uint i = 0; i < ft_strlen(s); ++i) {
        if (!ft_isnum(s[i])) {
            fprintf(stderr, "ft_ping: bad timing interval: %s\n", s);
            exit(EXIT_OTHER);
        }
    }
    return ft_strtol(s, NULL, 10);
}

static options_t parse_args(int ac, char** av) {
    options_t ret_val;
    ft_bzero(&ret_val, sizeof(ret_val));
    ret_val.i_set = false;
    bool prev_was_interval = false;
    for (int i = 1; i < ac; ++i) {
        if (prev_was_interval) {
            prev_was_interval = false;
            continue;
        }
        if (av[i][0] == '-') {
            for (unsigned int j = 1; j < ft_strlen(av[i]); ++j) {
                switch (av[i][j]) {
                    case 'a':
                        ret_val.alarm = true;
                        break;
                    case 'h':
                        help(0);
                        break;
                    case 'i':
                        if (ac <= i + 1) {
                            help('i');
                        }
                        prev_was_interval = true;
                        ret_val.interval = get_interval(av[i + 1]);
                        ret_val.i_set = true;
                        break;
                    case 'v':
                        ret_val.verbose = true;
                        break;
                    default:
                        help(av[i][j]);
                }
            }
        } else {
            if (ret_val.hostname != NULL) {
                help(0);
            }
            ret_val.hostname = av[i];
        }
    }
    if (ret_val.hostname == NULL) {
        fprintf(stderr, "ft_ping: usage error: Destination address required\n");
        exit(EXIT_ERR);
    }
    return ret_val;
}

static void end(stats_t* stats, const char* hostname, long ping_interval) {
    double percentage = (1 - (double)stats->recvd / (double)stats->sent) * 100;
    int prec = (int)percentage == percentage ? 0 : -1;

    double mean = stats->time_sum / (double)stats->recvd;
    double smean = stats->sum_time_p2 / (double)stats->recvd;
    stats->mdev = sqrt(smean - (mean * mean));
    stats->time_sum += (double)(1000 * ping_interval) * (double)(stats->sent - 1);

    printf("\n--- %s ft_ping statistics ---\n", hostname);
    printf("%d packets transmitted, %d received, %.*f%% packet loss, time %.0fms\n", stats->sent, stats->recvd, prec,
           percentage, stats->sent > 1 ? stats->time_sum : 0);
    printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", stats->min, stats->avg, stats->max, stats->mdev);
}

int main(int ac, char** av) {
    signal(SIGINT, sigint_handler);
    signal(SIGALRM, sigalrm_handler);

    options_t opts = parse_args(ac, av);
    struct addrinfo* ai = find_addr_info(&opts);
    const int sockfd = socket_setup();
    const static_info_t static_info = set_ip_info(ai, &opts);

    unsigned char buffer[PCKT_SIZE + sizeof(struct iphdr)];
    ft_bzero(buffer, PCKT_SIZE + sizeof(struct iphdr));
    struct icmphdr* icmp;
    icmp = (struct icmphdr*)buffer;

    received_msg_t received_msg;

    msg_init(icmp, &received_msg);
    printf("FT_PING %s (%s) %d(%lu) bytes of data.\n", opts.hostname, static_info.ip_addr_str, PCKT_SIZE,
           (PCKT_SIZE + sizeof(struct icmphdr) + sizeof(struct iphdr)));

    long ping_interval = opts.i_set ? opts.interval : INTERVAL;
    stats_t end_stats;
    ft_bzero(&end_stats, sizeof(end_stats));
    while (loop.active) {
        if (loop.send_next_msg) {
            loop.send_next_msg = false;
            alarm(ping_interval);
            ping(sockfd, ai, icmp, &received_msg, &static_info, &end_stats);
        }
    }

    freeaddrinfo(ai);
    close(sockfd);
    end(&end_stats, opts.hostname, ping_interval);
    return EXIT_OK;
}
