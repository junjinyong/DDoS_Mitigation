#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "udp.h"

int verify_puzzle(int puzzle);

int main(int argc, char *argv[]) {
    int sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t address_size;
    int puzzle;

    struct sockaddr_in server_address, user_address;

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) {
        error_handling("UDP socket creation error");
    }
    puts("Server socket creation successful");

    const int option = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_ip);
    server_address.sin_port = htons(atoi(server_port));

    if (bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        error_handling("bind() error");
    }
    puts("Server socket bind successful");

    while (1) {
        address_size = sizeof(user_address);
        str_len = (int) recvfrom(sock, message, BUF_SIZE, 0,
                           (struct sockaddr *) &user_address, &address_size);
        message[str_len] = '\0';
        puts("server received query from user");

        if(strcmp(message, "Q") == 0) {
            break;
        }

        const int verified = verify_puzzle(atoi(message));
        sprintf(message, "%d", verified);

        sendto(sock, message, str_len, 0,
               (struct sockaddr *) &user_address, address_size);
        puts("server sent response to user");
    }

    puts("All processes have completed");

    close(sock);
    getchar();
    return 0;
}

int verify_puzzle(int puzzle) {
    return (puzzle & 1023) == 0;
}
