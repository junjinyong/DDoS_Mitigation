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

    CBF* root = (CBF*) malloc(sizeof(CBF));
    (root -> address).sin_addr.s_addr = 0;
    (root -> address).sin_port = 0;
    root -> next = NULL;

    sleep(2);

    for (int i = 0; i < 10000; ++i) {
        printf("%d\n", i);
        address_size = sizeof(incoming_address);
        str_len = recvfrom(socket, message, BUFFER_SIZE, 0, (struct sockaddr*) &incoming_address, &address_size);
        message[str_len] = '\0';
        if (compare(&incoming_address, &auth_address)) {
            char* pos = message;
            const unsigned int dns_ip = strtoul(pos, &pos, 10);
            const unsigned int dns_port = strtoul(pos, &pos, 10);
            const unsigned int seed = strtoul(pos, &pos, 10);
            const unsigned int length = strtoul(pos, &pos, 10);

            CBF* prev = root;
            CBF* curr = root -> next;
            const struct sockaddr_in dns = create_address(dns_ip, dns_port);
            while (curr && !compare(&dns, &(curr -> address))) {
                prev = curr;
                curr = curr -> next;
            }
            if (curr == NULL) {
                CBF* node = (CBF*) malloc(sizeof(CBF));
                node -> address = dns;
                node -> next = NULL;
                prev -> next = node;
                curr = node;
            }

            int count = 0;
            for (CBF* t = root -> next; t != NULL; t = t -> next) {
                ++count;
            }

            const unsigned int salt = 42;
            unsigned int token = seed;
            for (int j = 0; j < length; ++j) {
                insert(curr, token);
                token = hash(salt, token);
            }
            printf("\n");
        } else {
            const unsigned int threshold = 2048;
            if (h(message) >= threshold) {
                printf("A\n");
                goto LABEL1;
            }

            char* pos = message;
            const unsigned int ip = strtoul(pos, &pos, 10);
            const unsigned int port = strtoul(pos, &pos, 10);
            const unsigned int dns_ip = strtoul(pos, &pos, 10);
            const unsigned int dns_port = strtoul(pos, &pos, 10);
            const unsigned int token = strtoul(pos, &pos, 10);

            const struct sockaddr_in user = create_address(ip, port);
            if (!compare(&user, &incoming_address)) {
                printf("B\n");
                goto LABEL1;
            }

            const struct sockaddr_in dns = create_address(dns_ip, dns_port);

            CBF* curr = root -> next;
            while(curr && !compare(&dns, &(curr -> address))) {
                curr = curr -> next;
            }
            if (curr == NULL) {
                printf("C\n");
                goto LABEL1;
            }

            unsigned int flag = 0;
            for (int j = 0; j < MAX_LENGTH; ++j) {
                if (test(curr, token)) {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) {
                printf("D\n");
                goto LABEL1;
            }

            sprintf(message, "valid");
            goto LABEL2;
LABEL1:
            sprintf(message, "invalid");
LABEL2:
            sendto(socket, message, strlen(message), 0, (struct sockaddr*) &incoming_address, sizeof(incoming_address));
        }
    }

    close(socket);
    getchar();
    return 0;
}
