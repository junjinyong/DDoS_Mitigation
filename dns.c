#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "udp.h"

int main(int argc, char *argv[]) {
    int sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t address_size;
    int puzzle;

    struct sockaddr_in dns_address, user_address;

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) {
        error_handling("UDP socket creation error");
    }
    puts("DNS socket creation successful");

    const int option = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    memset(&dns_address, 0, sizeof(dns_address));
    dns_address.sin_family = AF_INET;
    dns_address.sin_addr.s_addr = inet_addr(dns_ip);
    dns_address.sin_port = htons(atoi(dns_port));

    if (bind(sock, (struct sockaddr *) &dns_address, sizeof(dns_address)) == -1) {
        error_handling("bind() error");
    }
    puts("DNS socket bind successful");

    while (1) {
        address_size = sizeof(user_address);
        str_len = (int) recvfrom(sock, message, BUF_SIZE, 0,
                                 (struct sockaddr *) &user_address, &address_size);
        message[str_len] = '\0';
        puts("DNS received query from user");

        if(strcmp(message, "Q") == 0) {
            break;
        }

        sprintf(message, "%d", rand());

        sendto(sock, message, str_len, 0,
               (struct sockaddr *) &user_address, address_size);
        puts("DNS sent response to user");
    }

    puts("All processes have completed");

    close(sock);
    getchar();
    return 0;
}
