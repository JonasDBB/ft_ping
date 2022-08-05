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

options_t parse_args(int ac, char **av) {
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

unsigned short checksum(void* addr, size_t count) {
    unsigned short* buf = addr;
    register long sum = 0;
    while (count > 1) {
        sum += *buf++;
        count -= 2;
    }
    if (count > 0) {
        sum += *(unsigned char*)buf;
    }
    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    return ~sum;
}

struct addrinfo *find_addr_info(options_t *opts) {
    struct addrinfo hints;
    ft_bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;

    struct addrinfo* ai = NULL;
    int ret = getaddrinfo(opts->hostname[0], NULL, &hints, &ai);
    if (ret != 0) {
        freeaddrinfo(ai);
        char buf[28];
        switch (ret) {
            case EAI_NONAME:
                fprintf(stderr, "ft_ping: %s: Name or service not known\n", opts->hostname[0]);
                exit(EXIT_OTHER);
            case EAI_AGAIN:
                fprintf(stderr, "ft_ping: %s: Temporary failure in name resolution\n", opts->hostname[0]);
                exit(EXIT_OTHER);
            case EAI_MEMORY:
                fprintf(stderr, "ft_ping: %s: Memory allocation failure\n", opts->hostname[0]);
                exit(EXIT_OTHER);
            case EAI_SYSTEM:
                fprintf(stderr, "ft_ping: %s: System error returned in `errno'\n", opts->hostname[0]);
                exit(EXIT_OTHER);
            case EAI_OVERFLOW:
                fprintf(stderr, "ft_ping: %s: Argument buffer overflow\n", opts->hostname[0]);
                exit(EXIT_OTHER);
            default:
                ft_bzero(buf, 28);
                sprintf(buf, "getaddrinfo failed with %d", ret);
                fatal_err(buf);
        }
    }
    return ai;
}

int socket_setup() {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        fatal_err("socket failure");
    }

    unsigned char ttl_val = 64;
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0) {
        fatal_err("setsockopt err");
    }
    struct timeval tv_out;
    tv_out.tv_sec = 1;
    tv_out.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof(tv_out)) != 0) {
        fatal_err("setsockopt err");
    }
    return sockfd;
}

void send_icmp_message(int sockfd, struct icmphdr *icmp, struct timeval *before, struct addrinfo *ai) {
    static int counter = 0;
    icmp->checksum = 0;
    icmp->checksum = checksum(icmp, PCKT_SIZE);
    icmp->un.echo.sequence = counter++;
    gettimeofday(before, 0);
    ssize_t sendret = sendto(sockfd, icmp, PCKT_SIZE, 0, (struct sockaddr*)ai->ai_addr, sizeof(*ai->ai_addr));
    if (sendret < 0) {
        fatal_err("sendto error");
    }
}

int main(int ac, char **av)
{
    print_os_name();

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    signal(SIGINT, sigint_handler);
    options_t opts = parse_args(ac, av);
    struct addrinfo *ai = find_addr_info(&opts);
    int sockfd = socket_setup();

    struct timeval before;
    struct timeval after;
    struct icmphdr *icmp;
    unsigned char buffer[PCKT_SIZE + sizeof(struct iphdr)];
    ft_bzero(buffer, PCKT_SIZE + sizeof(struct iphdr));

    icmp = (struct icmphdr*)buffer;
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->checksum = 0;
    icmp->un.echo.id = getpid();

    send_icmp_message(sockfd, icmp, &before, ai);

    struct msghdr msg;
    char addrbuf[128];
    struct iovec iov;

    ft_bzero(&msg, sizeof(msg));
    iov.iov_base = (char*)buffer;
    iov.iov_len = PCKT_SIZE;

    msg.msg_name = addrbuf;
    msg.msg_namelen = sizeof(addrbuf);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    ssize_t recvret = recvmsg(sockfd, &msg, MSG_WAITALL);
    gettimeofday(&after, 0);
    if (recvret < 0) {
        fatal_err("recvmsg error");
    }

    unsigned char *buf = msg.msg_iov->iov_base;
    struct icmphdr *icp_reply = (struct icmphdr*)(buf + sizeof(struct iphdr));
    if (icp_reply->type != ICMP_ECHOREPLY) {
        printf("icmp reply type is wrong\nSTILL NEED TO HANDLE\n");
    }
    int csfailed = checksum(icp_reply, PCKT_SIZE);
    if (csfailed) {
        printf("bad checksum\nSTILL NEED TO HANDLE\n");
    }

    struct iphdr *ip;
    ip = (struct iphdr*)buf;
    double timedif = get_time_since_in_ms(&before, &after);
    char namebuf[INET_ADDRSTRLEN];
    ft_bzero(namebuf, INET_ADDRSTRLEN);
    struct sockaddr_in* ipn = (struct sockaddr_in*)ai->ai_addr;
    const char *ipname = inet_ntop(AF_INET, &ipn->sin_addr, namebuf, sizeof(namebuf));
    printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", PCKT_SIZE,
           ipname, icmp->un.echo.sequence, ip->ttl, timedif);

    freeaddrinfo(ai);
    end(av[0]);
    return 0;
}
