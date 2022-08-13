#ifndef FT_PING_FT_PING_H
#define FT_PING_FT_PING_H

#include <stdbool.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>

#define SILENCE_END
#define PCKT_SIZE 64
#define MAX_RECV_LEN 128

enum exitcode {
    EXIT_OK = 0,
    EXIT_ERR = 1,
    EXIT_OTHER = 2
};

typedef struct str_str_pair_s {
    const char* first;
    const char* second;
} str_str_pair_t;


typedef struct options_s {
    bool verbose;
    char* hostname[10];
    unsigned int nr_of_hosts;
} options_t;

typedef struct received_msg_s {
    struct msghdr msg_hdr;
    char iovbuf[MAX_RECV_LEN];
    struct iovec iov;
    struct iphdr* ip_hdr;
    struct icmphdr* icmp_reply;
} received_msg_t;

typedef struct static_info_s {
    struct timeval* start_time;
    char ip_addr_str[INET_ADDRSTRLEN];
    char host_name[NI_MAXHOST];
} static_info_t;

// ft_ping.c
void end(const char* hostname);

// utils.c
void sigint_handler(int param);
void sigalrm_handler(int i);
void fatal_err(const char* err);
float get_time_since_in_ms(const struct timeval* first, const struct timeval* second);

// ping_loop.c
void ping_loop(int sockfd, struct addrinfo* ai, struct icmphdr* icmp, received_msg_t* rec_msg, const static_info_t* info);

// setup.c
struct addrinfo* find_addr_info(options_t* opts);
int socket_setup();
static_info_t set_ip_info(struct addrinfo* ai);
void msg_init(struct icmphdr* icmp, received_msg_t* recv_msg);

// remove later
void send_sigint_to_self();
void print_os_name();


#endif //FT_PING_FT_PING_H
