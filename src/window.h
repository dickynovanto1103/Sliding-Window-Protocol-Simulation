#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <time.h>

class SenderWindow {
public:
    unsigned int size;
    int front;
    int back;
    unsigned int maxSize;
    unsigned char *data;
    bool *isACKed;
    clock_t *startTransferTime;

    SenderWindow(unsigned int _maxSize);
};

class ReceiverWindow {
public:
    unsigned int maxSize;
    unsigned char *data;
    bool *isReceived;

    ReceiverWindow(unsigned int _maxSize);
};

class ReceiverBuffer {
public:
    unsigned int size;
    int front;
    int back;
    unsigned int maxSize;
    unsigned char *data;
    unsigned int *seqnum;

    ReceiverBuffer(unsigned int _maxSize);
};

#endif
