#include "window.h"

SenderWindow::SenderWindow(unsigned int _maxSize) {
    size = 0;
    front = -1;
    back = -1;
    maxSize = _maxSize;
    data = new unsigned char[maxSize];
    isACKed = new bool[maxSize];
    startTransferTime = new clock_t[maxSize];

    for (unsigned int i = 0; i < maxSize; i++) {
        isACKed[i] = false;
        startTransferTime[i] = -1;
    }
}

ReceiverWindow::ReceiverWindow(unsigned int _maxSize) {
    maxSize = _maxSize;
    data = new unsigned char[maxSize];
    isReceived = new bool[maxSize];

    for (unsigned int i = 0; i < maxSize; i++) {
        isReceived[i] = false;
    }
}

ReceiverBuffer::ReceiverBuffer(unsigned int _maxSize) {
    size = 0;
    front = -1;
    back = -1;
    maxSize = _maxSize;
    data = new unsigned char[maxSize];
    seqnum = new unsigned int[maxSize];
}
