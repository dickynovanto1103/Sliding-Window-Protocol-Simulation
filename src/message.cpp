#include "message.h"

Message::Message() {
    soh = SOH;
    stx = STX;
    etx = ETX;
}

Message::Message(unsigned int _seqnum, unsigned char _data) {
    soh = SOH;
    seqnum = _seqnum;
    stx = STX;
    data = _data;
    etx = ETX;
}
