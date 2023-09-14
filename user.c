#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "udp.h"

int main(int argc, char *argv[]) {
    int server_sock, dns_socket;
    char server_message[BUF_SIZE], dns_message[BUF_SIZE];
    int str_len;
    socklen_t adr_sz;

    struct sockaddr_in serv_adr, from_adr;

    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    server_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (server_sock == -1) {
        error_handling("socket() error");
    }

    const int option = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    while (1) {
        fputs("Insert server_message(q to quit): ", stdout);
        fgets(server_message, sizeof(server_message), stdin);
        if (!strcmp(server_message, "q\n") || !strcmp(server_message, "Q\n")) {
            break;
        }

        sendto(server_sock, server_message, strlen(server_message), 0,
               (struct sockaddr *) &serv_adr, sizeof(serv_adr));
        adr_sz = sizeof(from_adr);
        str_len = (int) recvfrom(server_sock, server_message, BUF_SIZE, 0,
                                 (struct sockaddr *) &from_adr, &adr_sz);
        server_message[str_len] = 0;
        printf("Message from server: %s", server_message);
    }

    strcpy(server_message, "Q");
    sendto(server_sock, server_message, strlen(server_message), 0,
           (struct sockaddr *) &serv_adr, sizeof(serv_adr));

    close(server_sock);
    return 0;
}
