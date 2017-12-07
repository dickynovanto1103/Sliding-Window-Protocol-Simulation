#include "message.cpp"
#include "packet_validator.cpp"
#include "response.cpp"
#include "window.cpp"

#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define FILE_ENDING 0x26
#define SLEEP_TIME 100000

using namespace std;

string filename;

ReceiverWindow receiverWindow(256);
ReceiverBuffer receiverBuffer(256);

sockaddr_in senderAddress;
int senderAddressSize = sizeof(senderAddress);
int sock;

bool pushToBuffer(unsigned char character, int seqnum,
                  ReceiverBuffer &receiverBuffer) {
    if (receiverBuffer.size == receiverBuffer.maxSize)
        return false;

    if (receiverBuffer.size == 0) {
        receiverBuffer.front = 0;
    }
    receiverBuffer.back = (receiverBuffer.back + 1) % receiverBuffer.maxSize;
    receiverBuffer.data[receiverBuffer.back] = character;
    receiverBuffer.seqnum[receiverBuffer.back] = seqnum;
    receiverBuffer.size++;

    return true;
}

void *writeFromBuffer(void *) {
    while (true) {
        if (receiverBuffer.size == 0)
            continue;

        ofstream outputFile;
        outputFile.open(filename, ios_base::app);

        unsigned char character = receiverBuffer.data[receiverBuffer.front];
        outputFile << character;

        outputFile.close();

        receiverWindow.isReceived[receiverBuffer.seqnum[receiverBuffer.front]] =
            false;

        receiverBuffer.front =
            (receiverBuffer.front + 1) % receiverBuffer.maxSize;
        receiverBuffer.size--;

        if (receiverBuffer.size == 0) {
            receiverBuffer.front = -1;
            receiverBuffer.back = -1;
        }

        usleep(SLEEP_TIME);
    }
}

void sendResponse(Response response, int sock, struct sockaddr_in senderAddress,
                  int senderAddressSize) {
    if (sendto(sock, &response, sizeof(Response), 0,
               (struct sockaddr *)&senderAddress, senderAddressSize) == -1) {

        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    // receive arguments
    if (argc < 5) {
        exit(EXIT_FAILURE);
    }

    filename = argv[1];
    int windowsize = atoi(argv[2]);
    int buffersize = atoi(argv[3]);
    int port = atoi(argv[4]);

    // open socket
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        exit(EXIT_FAILURE);
    }

    // initialize receiver address
    sockaddr_in receiverAddress;
    receiverAddress.sin_family = AF_INET;
    receiverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    receiverAddress.sin_port = htons(port);

    // bind receiver address to socket
    if (bind(sock, (sockaddr *)&receiverAddress, sizeof(receiverAddress)) < 0) {
        exit(EXIT_FAILURE);
    }

    // empty output file
    ofstream outputFile;
    outputFile.open(filename, ios::out | ios::trunc);

    // create child thread for receiving message
    pthread_t child_thread;
    if (pthread_create(&child_thread, NULL, &writeFromBuffer, NULL) < 0) {
        exit(EXIT_FAILURE);
    }

    // send response after receiving packet
    Message message;

    while (true) {
        if (recvfrom(sock, &message, sizeof(Message), 0,
                     (struct sockaddr *)&senderAddress,
                     (socklen_t *)&senderAddressSize) == -1) {
            exit(EXIT_FAILURE);
        }

        PacketValidator validator;
        if (validator.isPacketValid(message) &&
            receiverBuffer.size < receiverWindow.maxSize) {
            unsigned char data = message.data;
            receiverWindow.data[message.seqnum] = data;

            if (!receiverWindow.isReceived[message.seqnum]) {
                if (pushToBuffer(receiverWindow.data[message.seqnum],
                                 message.seqnum, receiverBuffer)) {
                    receiverWindow.isReceived[message.seqnum] = true;
                }
            }

            Response response(message.seqnum, windowsize, message.checksum);
            response.ack = ACK;

            sendResponse(response, sock, senderAddress, senderAddressSize);

            if (data == FILE_ENDING)
                break;
        } else if (receiverBuffer.size >= receiverWindow.maxSize) {

        } else {
            Response response(message.seqnum, windowsize, message.checksum);
            response.ack = NAK;

            sendResponse(response, sock, senderAddress, senderAddressSize);
        }

        while (receiverBuffer.size > 0) {
            // waiting for receiverWindow to be empty
        }
    }

    return 0;
}
