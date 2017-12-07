#include "response.h"

Response::Response() {
}

Response::Response(unsigned int _seqnum, unsigned char _advertisedWindowSize,
                   unsigned char _checksum) {
    seqnum = _seqnum;
    advertisedWindowSize = _advertisedWindowSize;
    checksum = _checksum;
}
