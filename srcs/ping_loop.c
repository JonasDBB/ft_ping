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
}

void update_stats(stats_t* stats, double time) {
    if (stats->min == 0 && stats->max == 0 && stats->avg == 0) {
        stats->min = time;
        stats->max = time;
        stats->avg = time;
        stats->time_sum = time;
        stats->sum_time_p2 = time * time;
        return;
    }

    stats->min = stats->min > time ? time : stats->min;
    stats->max = stats->max < time ? time : stats->max;
    stats->avg = ((((double)stats->recvd - 1) * stats->avg) + time) / (double)stats->recvd;
    stats->time_sum += time;
    stats->sum_time_p2 += time * time;
}

void ping(int sockfd, struct addrinfo* ai, struct icmphdr* icmp, received_msg_t* rec_msg, const static_info_t* info,
          stats_t* end_stats) {
    struct timeval before;
    struct timeval after;

    ft_bzero(rec_msg->iovbuf, MAX_RECV_LEN);

    send_icmp_message(sockfd, icmp, &before, ai);
    ++end_stats->sent;
    ssize_t recvret = recvmsg(sockfd, &rec_msg->msg_hdr, MSG_WAITALL);
    gettimeofday(&after, 0);

    if (recvret < 0) {
        if (info->verbose) {
            fprintf(stderr, "Echo not received.\n");
        }
        return;
    }
    if (rec_msg->icmp_reply->type != ICMP_ECHOREPLY) {
        if (info->verbose) {
            switch (rec_msg->icmp_reply->type) {
                case ICMP_DEST_UNREACH:
                    fprintf(stderr, "Destination Unreachable.\n");
                    break;
                case ICMP_TIME_EXCEEDED:
                    fprintf(stderr, "Time exceeded.\n");
                    break;
                default:
                    fprintf(stderr, "Received icmp message was not echo reply but %d isntead.\n",
                            rec_msg->icmp_reply->type);
                    break;
            }
        }
        return;
    }
    if (checksum(rec_msg->icmp_reply, PCKT_SIZE)) {
        if (info->verbose) {
            fprintf(stderr, "Incorrect checksum in echo reply.\n");
        }
        return;
    }
    if (rec_msg->icmp_reply->un.echo.id != (htons(getpid()))) {
        if (info->verbose) {
            fprintf(stderr, "Echo meant for other process.");
        }
        return;
    }
    ++end_stats->recvd;

    double timedif = get_time_since_in_ms(&before, &after);
    update_stats(end_stats, timedif);

    printf("%c%lu bytes from %s (%s):""icmp_seq=%d ttl=%d time=%.2f ms\n", info->alarm, PCKT_SIZE + sizeof(*icmp),
           info->host_name, info->ip_addr_str, ntohs(icmp->un.echo.sequence) + 1, rec_msg->ip_hdr->ttl, timedif);
}
