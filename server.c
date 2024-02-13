#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#include "udp.h"
#include "cbf.h"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        error_handling("Invalid arguments");
    }
    const struct sockaddr_in server_address = initialize_address(argv[1], argv[2]);
    const struct sockaddr_in auth_address = initialize_address(argv[3], argv[4]);
    const int socket = create_socket(&server_address);

    char message[BUFFER_SIZE];
    struct sockaddr_in incoming_address;
    socklen_t address_size;
    ssize_t str_len;

    sleep(1);

    for (int i = 0; i < 10000; ++i) {
        address_size = sizeof(incoming_address);
        str_len = recvfrom(socket, message, BUFFER_SIZE, 0, (struct sockaddr*) &incoming_address, &address_size);
        sprintf(message, "%u", 100);
        sendto(socket, message, strlen(message), 0, (struct sockaddr*) &incoming_address, sizeof(incoming_address));
    }

    close(socket);
    getchar();
    return 0;
}
