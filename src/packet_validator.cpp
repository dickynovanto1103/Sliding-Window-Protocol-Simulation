#include "packet_validator.h"

unsigned char PacketValidator::getChecksum(Message message) {
    int answer = 0;
    answer += (int)message.soh;
    answer += (int)message.seqnum;
    answer += (int)message.stx;
    answer += (int)message.data;
    answer += (int)message.etx;
    answer %= 256;

    unsigned char checksum = 0;
    for (int i = 0; i < 8; i++) {
        if (!((1 << i) & answer)) {

            checksum += (1 << i);
        }
    }

    return checksum;
}

bool PacketValidator::isPacketValid(Message message) {
    int answer = 0;
    answer += (int)message.soh;
    answer += (int)message.seqnum;
    answer += (int)message.stx;
    answer += (int)message.data;
    answer += (int)message.etx;
    answer += (int)message.checksum;
    answer %= 256;

    unsigned int checksum = 0;
    for (int i = 0; i < 8; i++) {
        if (!((1 << i) & answer)) {
            checksum += (1 << i);
        }
    }

    return checksum == 0;
}
