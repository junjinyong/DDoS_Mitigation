#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

#define M 8192
#define K 4
#define MAX_LENGTH 32

unsigned int h(const char* input) {
    unsigned char output[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*) input, strlen(input), output);
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
    struct sockaddr_in address;
    unsigned int arr[M];
    struct CBF* next;
} typedef CBF;

void insert(CBF* cbf, unsigned int x) {
    for (int i = 0; i < K; ++i) {
        const unsigned int p = hash(i, x) % M;
        ++(cbf -> arr)[p];
    }
}

int test(CBF* cbf, unsigned int x) {
    for (int i = 0; i < K; ++i) {
        const unsigned int p = hash(i, x) % M;
        if ((cbf -> arr)[p] <= 0) {
            return 0;
        }
    }
    return 1;
}

int erase(CBF* cbf, unsigned int x) {
    if (test(cbf, x) != 0) {
        return 0;
    }
    for (int i = 0; i < K; ++i) {
        const unsigned int p = hash(i, x) % M;
        --(cbf -> arr)[p];
    }
    return 1;
}
