#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "udp.h"
#include "cbf.h"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        error_handling("Invalid arguments");
    }
    const struct sockaddr_in user_address = initialize_address(NULL, NULL);
    const struct sockaddr_in server_address = initialize_address(argv[1], argv[2]);
    const struct sockaddr_in dns_address = initialize_address(argv[3], argv[4]);
    const int socket = create_socket(NULL);

    char message[BUFFER_SIZE];
    ssize_t str_len;

    sleep(1);

    for (int i = 0; i < 10000; ++i) {
        printf("%d\n", i);
        sprintf(message, " ");
        sendto(socket, message, strlen(message), 0, (struct sockaddr*) &dns_address, sizeof(dns_address));
        str_len = recvfrom(socket, message, BUFFER_SIZE, 0, NULL, NULL);
        message[str_len] = '\0';
        const unsigned int token = strtoul(message, NULL, 10);
        printf("%s\n", message);

        unsigned int nonce = 0;
        while(1) {
            const unsigned int x = hash(token, nonce);
            if (x < 1024) {
                break;
            }
            ++nonce;
        }

        sprintf(message, "%u", nonce);

        sendto(socket, message, strlen(message), 0, (struct sockaddr*) &server_address, sizeof(server_address));
        str_len = recvfrom(socket, message, BUFFER_SIZE, 0, NULL, NULL);
        message[str_len] = '\0';
    }

    close(socket);
    getchar();
    return 0;
}
