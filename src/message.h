#ifndef _MESSAGE_
#define _MESSAGE_

#define SOH 0x1
#define STX 0x2
#define ETX 0x3

class Message {
public:
    unsigned char soh;
    unsigned int seqnum;
    unsigned char stx;
    unsigned char data;
    unsigned char etx;
    unsigned char checksum;

    Message();

    Message(unsigned int _seqnum, unsigned char _data);
};

#endif
