#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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

    sleep(1);

    for (int i = 0; i < 10000; ++i) {
        address_size = sizeof(incoming_address);
        str_len = recvfrom(socket1, message, BUFFER_SIZE, 0, (struct sockaddr*) &incoming_address, &address_size);
        sprintf(message, "%u %u", incoming_address.sin_addr.s_addr, incoming_address.sin_port);
        sendto(socket1, message, strlen(message), 0, (struct sockaddr*) &incoming_address, sizeof(incoming_address));
    }

    close(socket1);
    close(socket2);
    getchar();
    return 0;
}
