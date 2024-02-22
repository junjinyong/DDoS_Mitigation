
struct Bin {
    struct sockaddr_in address;
    time_t start;
    unsigned int count;
    struct Bin* next;
} typedef Bin;

int adjust(Bin* bin) {
    const unsigned int count = bin -> count;
    bin -> start = time(NULL);
    bin -> count = 0;

    int diff = 0;
    if (count > 9) {
        diff = 1;
    }
    if (count < 3) {
        diff = -1;
    }
    return diff;
}

int poll(Bin* bin) {
    int difficulty = 0;
    if (difftime(time(NULL), bin -> start) >= 3.0) {
        difficulty = adjust(bin);
    }
    ++(bin -> count);
    return difficulty;
}


