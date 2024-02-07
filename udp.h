#include <string.h>

#define BUF_SIZE 1024
#define CHAIN_LENGTH 128

#define NUM_DNS 4

void error_handling(const char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

const char* server_ip = "127.0.0.1";
const char* server_port = "10000";

const char* dns_ip_list[NUM_DNS] = {"127.0.0.1", "127.0.0.1", "127.0.0.1", "127.0.0.1"};
const char* dns_port_list[NUM_DNS] = {"20000", "21000", "22000", "23000"};

const char* auth_ip = "127.0.0.1";
const char* auth_port = "30000";

const char* mon_ip = "127.0.0.1";
const char* mon_port = "40000";

const int option = 1;

struct sockaddr_in server_address, dns_address[NUM_DNS], auth_address, mon_address;

void init(struct sockaddr_in* address, const char* ip, const char* port) {
    memset(address, 0, sizeof(struct sockaddr_in));
    address -> sin_family = AF_INET;
    (address -> sin_addr).s_addr = inet_addr(ip);
    address -> sin_port = htons(strtol(port, NULL, 10));
}

int initialize(const struct sockaddr_in* address) {
    init(&server_address, server_ip, server_port);
    for (int i = 0; i < NUM_DNS; ++i) {
        init(&dns_address[i], dns_ip_list[i], dns_port_list[i]);
    }
    init(&auth_address, auth_ip, auth_port);
    init(&mon_address, mon_ip, mon_port);
    const int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) {
        error_handling("Socket creation error");
    }
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));
    if (address != NULL && bind(sock, (const struct sockaddr *) address, sizeof(struct sockaddr_in)) == -1) {
        error_handling("Bind error");
    }
    return sock;
}

int compare(const struct sockaddr_in *a, const struct sockaddr_in *b) {
    return (a -> sin_addr).s_addr == (b -> sin_addr).s_addr && (a -> sin_port) == (b -> sin_port);
}
