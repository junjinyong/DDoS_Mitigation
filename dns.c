#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#include "udp.h"
#include "cbf.h"

int main(int argc, char *argv[]) {
    if (argc != 7) {
        error_handling("Invalid arguments");
    }
    const struct sockaddr_in dns_address1 = initialize_address(argv[1], argv[2]);
    const struct sockaddr_in dns_address2 = initialize_address(argv[3], argv[4]);
    const struct sockaddr_in auth_address = initialize_address(argv[5], argv[6]);
    const int socket1 = create_socket(&dns_address1);
    const int socket2 = create_socket(&dns_address2);

    char message[BUFFER_SIZE];
    struct sockaddr_in incoming_address;
    socklen_t address_size;
    ssize_t str_len;

    const time_t start_time = time(NULL);
    while(1) {
        const double elapsed_time = difftime(time(NULL), start_time);
        if (elapsed_time >= 1.0) {
            break;
        }
        address_size = sizeof(incoming_address);
        str_len = recvfrom(socket1, message, BUFFER_SIZE, 0, (struct sockaddr*) &incoming_address, &address_size);
        message[str_len] = '\0';
        const unsigned int ip = incoming_address.sin_addr.s_addr;
        const unsigned int port = incoming_address.sin_port;
        sprintf(message, "%u %u", ip, port);
        sendto(socket1, message, strlen(message), 0, (struct sockaddr*) &incoming_address, sizeof(incoming_address));
    }

    int index = -1;
    unsigned int hash_chain[MAX_LENGTH];

    for (int i = 0; i < 10000; ++i) {
        address_size = sizeof(incoming_address);
        str_len = recvfrom(socket1, message, BUFFER_SIZE, 0, (struct sockaddr*) &incoming_address, &address_size);
        message[str_len] = '\0';
        printf("index: %d\n", index);
        if (index < 0) {
            const unsigned int dns_ip = dns_address1.sin_addr.s_addr;
            const unsigned int dns_port = dns_address1.sin_port;
            sprintf(message, "%u %u", dns_ip, dns_port);
            sendto(socket2, message, strlen(message), 0, (struct sockaddr*) &auth_address, sizeof(auth_address));

            str_len = recvfrom(socket2, message, BUFFER_SIZE, 0, NULL, NULL);
            message[str_len] = '\0';
            char* pos = message;
            const unsigned int seed = strtoul(pos, &pos, 10);
            const int length = (int) strtol(pos, NULL, 10);

            hash_chain[0] = seed;
            const unsigned int salt = 42;
            for (int j = 0; j + 1 < length; ++j) {
                hash_chain[j + 1] = hash(salt, hash_chain[j]);
            }
            index = length - 1;
        }
        sprintf(message, "%u %u", hash_chain[index--], 2048);
        sendto(socket1, message, strlen(message), 0, (struct sockaddr*) &incoming_address, sizeof(incoming_address));
    }

    close(socket1);
    close(socket2);
    getchar();
    return 0;
}
