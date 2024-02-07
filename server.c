#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#include "udp.h"
#include "cbf.h"

int verify_puzzle(int puzzle);
int monitor();

int main(int argc, char *argv[]) {
    const int sock = initialize(&server_address);
    char message[BUF_SIZE];
    int str_len;
    socklen_t address_size;
    unsigned int ip, port, dns_ip, dns_port, token, nonce;
    unsigned int threshold = 64;

    struct sockaddr_in incoming_address;

    while (1) {
        address_size = sizeof(incoming_address);
        str_len = (int) recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr *) &incoming_address, &address_size);
        if(strcmp(message, "Q") == 0) {
            break;
        }

        sscanf(message, "%u %u %u %u %u %u", &ip, &port, &dns_ip, &dns_port, &token, &nonce);
        const unsigned int x = h(ip, port, dns_ip, dns_port, token, nonce);
        if (x < threshold) {
            printf("@");
        } else {
            printf("#");
        }

        sprintf(message, " ");
        address_size = sizeof(incoming_address);
        sendto(sock, message, strlen(message), 0,(struct sockaddr *) &incoming_address, address_size);

    }

    close(sock);
    getchar();
    return 0;
}

int monitor() {
    static long long int traffic = 0;
    static time_t prev = 0;

    ++traffic;
    time_t curr = time(NULL);
    if(curr - prev >= 3) {
        const int result = (traffic >= 100);
        traffic = 0;
        prev = curr;
        return result ? 1 : -1;
    }
    return 0;
}
