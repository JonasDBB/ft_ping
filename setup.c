#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "ft_ping.h"
#include "clib/ft_clib.h"

struct addrinfo* find_addr_info(options_t* opts) {
    struct addrinfo hints;
    ft_bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;

    struct addrinfo* ai = NULL;
    int ret = getaddrinfo(opts->hostname, NULL, &hints, &ai);
    if (ret != 0) {
        freeaddrinfo(ai);
        char buf[28];
        switch (ret) {
            case EAI_NONAME:
                fprintf(stderr, "ft_ping: %s: Name or service not known\n", opts->hostname);
                exit(EXIT_OTHER);
            case EAI_AGAIN:
                fprintf(stderr, "ft_ping: %s: Temporary failure in name resolution\n", opts->hostname);
                exit(EXIT_OTHER);
            case EAI_MEMORY:
                fprintf(stderr, "ft_ping: %s: Memory allocation failure\n", opts->hostname);
                exit(EXIT_OTHER);
            case EAI_SYSTEM:
                fprintf(stderr, "ft_ping: %s: System error returned in `errno'\n", opts->hostname);
                exit(EXIT_OTHER);
            case EAI_OVERFLOW:
                fprintf(stderr, "ft_ping: %s: Argument buffer overflow\n", opts->hostname);
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
//    struct timeval timeout;
//    timeout.tv_sec = 1;
//    timeout.tv_usec = 0;
//    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) != 0) {
//        fatal_err("setsockopt err");
//    }
    return sockfd;
}

static_info_t set_ip_info(struct addrinfo* ai, options_t* opts)
{
    static_info_t ret;

    ret.alarm = opts->alarm ? '\a' : '\0';
    ft_bzero(ret.ip_addr_str, INET_ADDRSTRLEN);
    struct sockaddr_in* ipn = (struct sockaddr_in*)ai->ai_addr;
    inet_ntop(AF_INET, &ipn->sin_addr, ret.ip_addr_str, sizeof(ret.ip_addr_str));

    ft_bzero(ret.host_name, NI_MAXHOST);
    getnameinfo(ai->ai_addr, sizeof(*ai->ai_addr), ret.host_name, sizeof(ret.host_name), 0, 0, 0);

    return ret;
}

void msg_init(struct icmphdr* icmp, received_msg_t* recv_msg) {
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->checksum = 0;
    icmp->un.echo.id = htons(getpid());

    ft_bzero(&recv_msg->msg_hdr, sizeof(struct msghdr));
    ft_bzero(recv_msg->iovbuf, 128);
    recv_msg->iov.iov_base = recv_msg->iovbuf;
    recv_msg->iov.iov_len = 128;
    recv_msg->msg_hdr.msg_iov = &recv_msg->iov;
    recv_msg->msg_hdr.msg_iovlen = 1;

    recv_msg->ip_hdr = (struct iphdr*)(recv_msg->iovbuf);
    recv_msg->icmp_reply = (struct icmphdr*)(recv_msg->iovbuf + sizeof(struct iphdr));

}