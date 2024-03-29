#ifndef FT_PING_FT_PING_H
#define FT_PING_FT_PING_H

#include <stdbool.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>

#define PCKT_SIZE 56
#define MAX_RECV_LEN 128
#define INTERVAL 1

enum exitcode {
    EXIT_OK = 0,
    EXIT_ERR = 1,
    EXIT_OTHER = 2
};

typedef struct str_str_pair_s {
    const char* first;
    const char* second;
} str_str_pair_t;

typedef struct loop_s {
    bool active;
    bool send_next_msg;
} loop_t;

typedef struct options_s {
    bool verbose;
    bool alarm;
    long interval;
    char* hostname;
    bool i_set;
} options_t;

typedef struct received_msg_s {
    struct msghdr msg_hdr;
    char iovbuf[MAX_RECV_LEN];
    struct iovec iov;
    struct iphdr* ip_hdr;
    struct icmphdr* icmp_reply;
} received_msg_t;

typedef struct static_info_s {
    char ip_addr_str[INET_ADDRSTRLEN];
    char dest_str[INET_ADDRSTRLEN + NI_MAXHOST + 3];
    char alarm;
    bool verbose;
} static_info_t;

typedef struct stats_s {
    unsigned int sent;
    unsigned int recvd;
    double min;
    double max;
    double avg;
    double mdev;
    double time_sum;
    double sum_time_p2;
} stats_t;

// utils.c
void sigint_handler(int i);
void sigalrm_handler(int i);
void fatal_err(const char* err);
double get_time_since_in_ms(const struct timeval* first, const struct timeval* second);
bool is_valid_ipv4_address(const char* s);

// ping_loop.c
void ping(int sockfd, struct addrinfo* ai, struct icmphdr* icmp, received_msg_t* rec_msg, const static_info_t* info,
          stats_t* end_stats);

// setup.c
struct addrinfo* find_addr_info(options_t* opts);
int socket_setup();
static_info_t set_ip_info(struct addrinfo* ai, options_t* opts, bool arg_is_ipv4addr);
void msg_init(struct icmphdr* icmp, received_msg_t* recv_msg);

#endif //FT_PING_FT_PING_H
