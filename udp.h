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

#endif //DDOS_MITIGATION_UDP_H
