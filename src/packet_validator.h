#ifndef _PACKET_VALIDATOR_H_
#define _PACKET_VALIDATOR_H_

#include "message.h"

class PacketValidator {
public:
    unsigned char getChecksum(Message message);

    bool isPacketValid(Message message);
};

#endif
