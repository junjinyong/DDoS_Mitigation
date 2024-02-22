#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#include "udp.h"
#include "cbf.h"
#include "monitor.h"

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
    root -> address = create_address(0, 0);
    root -> next = NULL;

    Bin* b = (Bin*) malloc(sizeof(Bin));
    b -> next = NULL;

    sleep(1);

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
            const struct sockaddr_in dns = create_address(dns_ip, dns_port);

            CBF* prev = root;
            CBF* curr = root -> next;
            while (curr && !compare(&dns, &(curr -> address))) {
                prev = curr;
                curr = curr -> next;
            }
            if (curr == NULL) {
                CBF* node = (CBF*) malloc(sizeof(CBF));
                node -> address = dns;
                node -> next = NULL;
                node -> threshold = DEFAULT_THRESHOLD;
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
            char* pos = message;
            const unsigned int ip = strtoul(pos, &pos, 10);
            const unsigned int port = strtoul(pos, &pos, 10);
            const unsigned int dns_ip = strtoul(pos, &pos, 10);
            const unsigned int dns_port = strtoul(pos, &pos, 10);
            const unsigned int token = strtoul(pos, &pos, 10);
            const struct sockaddr_in dns = create_address(dns_ip, dns_port);

            const struct sockaddr_in user = create_address(ip, port);
            if (!compare(&user, &incoming_address)) {
                printf("B\n");
                goto LABEL1;
            }

            CBF* curr = root -> next;
            while(curr && !compare(&dns, &(curr -> address))) {
                curr = curr -> next;
            }
            if (curr == NULL) {
                printf("C\n");
                goto LABEL1;
            }

            const unsigned int threshold = curr -> threshold;
            if (h(message) >= threshold) {
                printf("A\n");
                goto LABEL1;
            }

            if (erase(curr, token) == 0) {
                printf("D\n");
                goto LABEL1;
            }

            Bin* p = b;
            Bin* c = b -> next;
            while(c && !compare(&dns, &(c -> address))) {
                p = c;
                c = c -> next;
            }
            if (c == NULL) {
                Bin* n = (Bin*) malloc(sizeof(Bin));
                n -> address = dns;
                n -> start = time(NULL);
                n -> count = 0;
                n -> next = NULL;
                p -> next = n;
                c = n;
            }
            const int difficulty = poll(c);
            if (difficulty) {
                printf("Difficulty: %u -> ", curr -> threshold);
                unsigned int result;
                if (difficulty == 1) {
                    result = (curr -> threshold + 1) >> 1;
                }
                if (difficulty == -1 && threshold < (2 << 30)) {
                    result = (curr -> threshold) << 1;
                }
                curr -> threshold = result;
                printf("%u\n", result);

                sprintf(message, "%u", result);
                sendto(socket, message, strlen(message), 0, (struct sockaddr*) &dns, sizeof(dns));

            }

            sprintf(message, "valid");

            goto LABEL2;
LABEL1:
            sprintf(message, "invalid");
LABEL2:
            sendto(socket, message, strlen(message), 0, (struct sockaddr*) &incoming_address, sizeof(incoming_address));
        }
    }

    CBF* curr = root;
    while(curr) {
        CBF* prev = curr;
        curr = curr -> next;
        free(prev);
    }

    close(socket);
    getchar();
    return 0;
}
