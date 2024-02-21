#include <string.h>
#include <stdlib.h>

void error_handling(const char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

#define BUFFER_SIZE 1024

const int option = 1;

struct sockaddr_in initialize_address(const char* ip, const char* port) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = ip ? inet_addr(ip) : htonl(INADDR_ANY);
    address.sin_port = port ? htons(strtol(port, NULL, 10)) : htons(0);
    return address;
}

int create_socket(const struct sockaddr_in* address) {
    // Create new UDP socket
    const int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) {
        error_handling("Socket creation error");
    }

    // Enable reuse of TIME_WAIT socket
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));
    if (address == NULL) {
        return sock;
    }

    // Bind socket to given address
    const int result = bind(sock, (const struct sockaddr *) address, sizeof(struct sockaddr_in));
    if (result == -1) {
        error_handling("Bind error");
    }
    return sock;
}

struct sockaddr_in create_address(const unsigned int ip, const unsigned int port) {
    struct sockaddr_in address;
    address.sin_addr.s_addr = ip;
    address.sin_port = port;
    return address;
}

int compare(const struct sockaddr_in *a, const struct sockaddr_in *b) {
    return (a -> sin_addr).s_addr == (b -> sin_addr).s_addr && (a -> sin_port) == (b -> sin_port);
}
