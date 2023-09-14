//
// Created by junjinyong on 23. 9. 14.
//

#ifndef DDOS_MITIGATION_UDP_H
#define DDOS_MITIGATION_UDP_H

#define BUF_SIZE 1024

void error_handling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

const char* server_ip = "127.0.0.1";
const char* server_port = "10000";

const char* dns_ip = "127.0.0.1";
const char* dns_port = "10001";

#endif //DDOS_MITIGATION_UDP_H
