#ifndef _RESPONSE_
#define _RESPONSE_

#define ACK 0x6
#define NAK 0x15

class Response {
public:
    unsigned char ack;
    unsigned int seqnum;
    unsigned char advertisedWindowSize;
    unsigned char checksum;

    Response();

    Response(unsigned int _seqnum, unsigned char _advertisedWindowSize,
             unsigned char _checksum);
};

#endif
