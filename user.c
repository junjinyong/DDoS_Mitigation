#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "udp.h"
#include "cbf.h"

int solve_puzzle(int token);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        exit(1);
    }
    const int index = (int) strtol(argv[1], NULL, 10);
    const int sock = initialize(NULL);
    const unsigned int dns_ip = dns_address[index].sin_addr.s_addr;
    const unsigned int dns_port = dns_address[index].sin_port;
    const unsigned int ip = rand(); // get ip;
    const unsigned int port = rand(); // get port;

    char message[BUF_SIZE];
    unsigned int str_len;
    socklen_t address_size;
    unsigned int token;
    unsigned int nonce;
    unsigned int threshold;

    struct sockaddr_in incoming_address;

    int iter = 10000;
    while (iter--) {
        // Query token and threshold
        printf("%d\n", iter);

        address_size = sizeof(incoming_address);
        sprintf(message, " ");
        address_size = sizeof(dns_address[index]);
        sendto(sock, message, strlen(message), 0, (struct sockaddr *) &(dns_address[index]), address_size);
        str_len = (unsigned int) recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr *) &incoming_address, &address_size);
        sscanf(message, "%u %u", &token, &threshold);

        // Solve puzzle
        nonce = 0;
        while (1) {
            const unsigned int x = h(ip, port, dns_ip, dns_port, token, nonce);
            if (x < threshold) {
                break;
            }
            ++nonce;
        }

        // Query server
        sprintf(message, "%u %u %u %u %u %u", ip, port, dns_ip, dns_port, token, nonce);
        address_size = sizeof(server_address);
        sendto(sock, message, strlen(message), 0,(struct sockaddr *) &server_address, address_size);
        str_len = (unsigned int) recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr *) &incoming_address, &address_size);
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
