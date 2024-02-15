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
    const struct sockaddr_in server_address = initialize_address(argv[1], argv[2]);
    const struct sockaddr_in dns_address = initialize_address(argv[3], argv[4]);
    const int socket = create_socket(NULL);

    char message[BUFFER_SIZE];
    ssize_t str_len;

    sleep(1);

    sprintf(message, " ");
    sendto(socket, message, strlen(message), 0, (struct sockaddr*) &dns_address, sizeof(dns_address));
    str_len = recvfrom(socket, message, BUFFER_SIZE, 0, NULL, NULL);
    message[str_len] = '\0';
    char* pos;
    const unsigned int ip = strtoul(message, &pos, 10);
    const unsigned int port = strtoul(pos, NULL, 10);
    const unsigned int dns_ip = dns_address.sin_addr.s_addr;
    const unsigned int dns_port = dns_address.sin_port;
    printf("User\nip: %u\nport: %u\n\n", ip, port);
    printf("DNS\nip: %u\nport: %u\n\n", dns_ip, dns_port);

    sleep(2);

    for (int i = 0; i < 10000; ++i) {
        printf("%d ", i);
        sprintf(message, " ");
        sendto(socket, message, strlen(message), 0, (struct sockaddr*) &dns_address, sizeof(dns_address));
        str_len = recvfrom(socket, message, BUFFER_SIZE, 0, NULL, NULL);
        message[str_len] = '\0';
        const unsigned int token = strtoul(message, &pos, 10);
        const unsigned int threshold = strtoul(pos, NULL, 10);

        unsigned int nonce = 0;
        while(1) {
            sprintf(message, "%u %u %u %u %u %u", ip, port, dns_ip, dns_port, token, nonce);
            if (h(message) < threshold) {
                break;
            }
            ++nonce;
        }

        sendto(socket, message, strlen(message), 0, (struct sockaddr*) &server_address, sizeof(server_address));
        str_len = recvfrom(socket, message, BUFFER_SIZE, 0, NULL, NULL);
        message[str_len] = '\0';
        printf("%s\n", message);
    }

    close(socket);
    getchar();
    return 0;
}
