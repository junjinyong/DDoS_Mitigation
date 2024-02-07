#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "udp.h"
#include "cbf.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        exit(1);
    }
    const int index = (int) strtol(argv[1], NULL, 10);
    const int sock = initialize(&dns_address[index]);
    char message[BUF_SIZE];
    unsigned int str_len;
    socklen_t address_size;
    unsigned int seed;
    int length = -1;
    unsigned int hash_chain[CHAIN_LENGTH];
    unsigned int token, threshold;

    struct sockaddr_in incoming_address;

    while (1) {
        str_len = (unsigned int) recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr *) &incoming_address, &address_size);
        if (compare(&incoming_address, &auth_address)) {

        } else {
            if (length < 0) {
                //sprintf(message, "");

                seed = 0; //
                hash_chain[0] = seed;
                for (int i = 1; i < CHAIN_LENGTH; ++i) {
                    hash_chain[i] = hash(hash_chain[i - 1], hash_chain[i - 1]);
                }
                length = CHAIN_LENGTH - 1;
            }

            token = hash_chain[length--];
            threshold = 2048; //
            sprintf(message, "%u %u", token, threshold);
            address_size = sizeof(incoming_address);
            sendto(sock, message, strlen(message), 0,(struct sockaddr *) &incoming_address, address_size);
        }


    }

    puts("All processes have completed");

    close(sock);
    getchar();
    return 0;
}
