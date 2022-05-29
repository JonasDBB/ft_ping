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

enum exitcode {
    EXIT_OK = 0,
    EXIT_ERR = 1,
    EXIT_OTHER = 2
};

typedef struct s_options {
    bool verbose;
    char* hostname;
} options_t;

void help(char invalid_flag) {
    if (invalid_flag) {
        printf("ft_ping: invalid option -- '%c'\n", invalid_flag);
    }
    printf("\nUsage\n"
           "  ft_ping [options] <destination>\n"
           "Options:\n"
           "  %-15sdns name or ip address\n"
           "  %-15sprint help and exit\n"
           "  %-15sverbose output\n",
           "<destination>", "-h", "-v");
    exit(EXIT_OTHER);
}

options_t parse_args(int ac, char **av) {
    options_t ret_val;
    ret_val.verbose = false;
    ret_val.hostname = NULL;
    for (int i = 1; i < ac; ++i) {
        if (av[i][0] == '-') {
            for (int j = 1; j < strlen(av[i]); ++j) {
                switch (av[i][j])
                {
                    case 'h':
                        help(0);
                    case 'v':
                        ret_val.verbose = true;
                        break;
                    default:
                        help(av[i][j]);
                }
            }
        } else {
            if (not ret_val.hostname) {
                ret_val.hostname = av[i];
            }
        }
    }
    if (not ret_val.hostname) {
        printf("ft_ping: usage error: Destination address required\n");
        exit(EXIT_ERR);
    }
    return ret_val;
}

void sigint_handler(int param) {
    // print result
    exit(EXIT_OK);
}

int main(int ac, char **av)
{
    signal(SIGINT, sigint_handler);
    options_t opts = parse_args(ac, av);
    printf("verbose is %s\n", opts.verbose == true ? "true" : "false");
    printf("dest is %s\n", opts.hostname);
	return 0;
}
