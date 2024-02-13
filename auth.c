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
    const struct sockaddr_in auth_address = initialize_address(argv[1], argv[2]);
    const struct sockaddr_in server_address = initialize_address(argv[3], argv[4]);
    const int socket = create_socket(&auth_address);

    sleep(1);

    close(socket);
    getchar();
    return 0;
}
