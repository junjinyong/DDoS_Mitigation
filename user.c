#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "udp.h"

int solve_puzzle(int token);

int main(int argc, char *argv[]) {
    int sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t address_size;
    int puzzle = rand();

    struct sockaddr_in incoming_address;
    initialize_address();

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1) {
        error_handling("sock() error");
    }
    puts("User socket creation successful");

    const int option = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    int iter = 1000000;
    while (iter--) {
        sprintf(message, "%d", puzzle);

        puts("User queried server");
        sendto(sock, message, strlen(message), 0,
               (struct sockaddr *) &server_address, sizeof(server_address));

        do {
            address_size = sizeof(incoming_address);
            str_len = (int) recvfrom(sock, message, BUF_SIZE, 0,
                                     (struct sockaddr *) &incoming_address, &address_size);
            message[str_len] = '\0';
        } while(!compare(&server_address, &incoming_address));
        puts("User received response from server");


        sprintf(message, "%d", 1);
        sendto(sock, message, strlen(message), 0,
               (struct sockaddr *) &dns_address, sizeof(dns_address));
        puts("User queried DNS");

        do {
            address_size = sizeof(incoming_address);
            str_len = (int) recvfrom(sock, message, BUF_SIZE, 0,
                                     (struct sockaddr *) &incoming_address, &address_size);
            message[str_len] = '\0';
        } while(!compare(&dns_address, &incoming_address));
        puts("User received response from DNS");

        const int token = atoi(message);
        printf("token: %d\n", token);
        if(token == -1) {
            sleep(1);
        } else {
            puzzle = solve_puzzle(token);
            puts("User Puzzle Solving Successful");
        }
    }

    puts("All processes have completed");
    strcpy(message, "Q");
    sendto(sock, message, strlen(message), 0,
           (struct sockaddr *) &server_address, sizeof(server_address));
    sendto(sock, message, strlen(message), 0,
           (struct sockaddr *) &dns_address, sizeof(dns_address));
    sendto(sock, message, strlen(message), 0,
           (struct sockaddr *) &dns_address, sizeof(dns_address));

    close(sock);
    getchar();
    return 0;
}

int solve_puzzle(int token) {
    while((token & 1023) != 0) {
        ++token;
    }
    return token;
}
