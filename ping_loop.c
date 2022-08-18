#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>

#include "ft_ping.h"
#include "ft_clib.h"

static unsigned short checksum(void* addr, size_t count) {
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

static void send_icmp_message(int sockfd, struct icmphdr* icmp, struct timeval* before, struct addrinfo* ai) {
    static uint16_t counter = 0;
    icmp->checksum = 0;
    icmp->un.echo.sequence = htons(counter++);
    icmp->checksum = checksum(icmp, PCKT_SIZE);

    gettimeofday(before, 0);
    ssize_t sendret = sendto(sockfd, icmp, PCKT_SIZE, 0, (struct sockaddr*)ai->ai_addr, sizeof(*ai->ai_addr));
    if (sendret < 0) {
        fatal_err("sendto error");
    }
    LOG("sendret %d", sendret);
}

void ping(int sockfd, struct addrinfo* ai, struct icmphdr* icmp, received_msg_t* rec_msg, const static_info_t* info) {
    struct timeval before;
    struct timeval after;

    send_icmp_message(sockfd, icmp, &before, ai);
    ssize_t recvret = recvmsg(sockfd, &rec_msg->msg_hdr, MSG_WAITALL);
    LOG("recvret %d", recvret);
    gettimeofday(&after, 0);
    if (recvret < 0) {
        fatal_err("recvmsg error");
    }

    if (rec_msg->icmp_reply->type != ICMP_ECHOREPLY) {
        fprintf(stderr, "icmp reply type is wrong\nNO ERROR YET\n");
    }
    int csfailed = checksum(rec_msg->icmp_reply, PCKT_SIZE);
    if (csfailed) {
        fprintf(stderr, "bad checksum\nNO ERROR YET\n");
    }
    if (rec_msg->icmp_reply->un.echo.id != (htons(getpid()))) {
        fprintf(stderr, "echo meant for other process\nNO ERROR YET\n");
    }

    double timedif = get_time_since_in_ms(&before, &after);

    if (info->alarm) {
        printf("\a");
    }
    printf(CLR_GRN"%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%.2f ms"CLR_RESET"\n", PCKT_SIZE,
           info->host_name, info->ip_addr_str, icmp->un.echo.sequence + 1, rec_msg->ip_hdr->ttl, timedif);
}
