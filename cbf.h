#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

#define M 8192
#define K 4

unsigned int h(unsigned int user_ip, unsigned int user_port, unsigned int dns_ip, unsigned int dns_port, unsigned int token, unsigned int nonce) {
    unsigned char input[128];
    unsigned char output[SHA256_DIGEST_LENGTH];
    sprintf((char*) input, "%u %u %u %u %u %u", user_ip, user_port, dns_ip, dns_port, token, nonce);
    SHA256((const unsigned char *) input, strlen((char*) input), output);
    return *((unsigned int*) output);
}

unsigned int hash(unsigned int i, unsigned int x) {
    unsigned char input[64];
    unsigned char output[SHA256_DIGEST_LENGTH];
    sprintf((char*) input, "%u %u", i, x);
    SHA256((const unsigned char *) input, strlen((char*) input), output);
    return *((unsigned int*) output);
}

struct CBF {
    unsigned int arr[M];
} typedef CBF;

void insert(CBF* cbf, unsigned int x) {
    for (int i = 0; i < K; ++i) {
        const unsigned int p = hash(i, x) % M;
        ++(cbf -> arr)[p];
    }
}

int test(CBF* cbf, int x) {
    for (int i = 0; i < K; ++i) {
        const unsigned int p = hash(i, x) % M;
        if ((cbf -> arr)[p] <= 0) {
            return 0;
        }
    }
    return 1;
}

int erase(CBF* cbf, int x) {
    if (test(cbf, x) != 0) {
        return 0;
    }
    for (int i = 0; i < K; ++i) {
        const unsigned int p = hash(i, x) % M;
        --(cbf -> arr)[p];
    }
    return 1;
}
