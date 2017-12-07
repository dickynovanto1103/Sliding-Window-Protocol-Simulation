#include "message.cpp"
#include "packet_validator.cpp"
#include "response.cpp"
#include "window.cpp"

#include <arpa/inet.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

#define FILE_ENDING 0x26
#define TIMEOUT 1000

SenderWindow senderWindow(256);

struct sockaddr_in receiverAddress;
int receiverAddressSize = sizeof(receiverAddress);
int sock;

void sendMessage(int seqnum, unsigned char data, int sock,
                 sockaddr_in receiverAddress, int receiverAddressSize) {
    Message message(seqnum, data);

    PacketValidator validator;
    message.checksum = validator.getChecksum(message);

    sendto(sock, &message, sizeof(Message), 0,
           (struct sockaddr *)&receiverAddress, receiverAddressSize);
}

// receive response from recvfile
void *recvResponse(void *) {
    Response response;
    while (true) {
        if (recvfrom(sock, &response, sizeof(Response), 0,
                     (struct sockaddr *)&receiverAddress,
                     (socklen_t *)&receiverAddressSize) == -1) {

            exit(EXIT_FAILURE);
        }

        if (response.ack == ACK) {
            senderWindow.isACKed[response.seqnum] = true;
        } else {
            // resend packet
            int seqnum = response.seqnum;
            int data = senderWindow.data[seqnum];

            senderWindow.startTransferTime[seqnum] = clock();
            sendMessage(seqnum, data, sock, receiverAddress,
                        receiverAddressSize);
        }
    }
}

void addToBack(unsigned char character, SenderWindow &senderWindow) {
    if (senderWindow.size == 0) {
        senderWindow.front = 0;
    }
    senderWindow.back = (senderWindow.back + 1) % senderWindow.maxSize;
    senderWindow.data[senderWindow.back] = character;
    senderWindow.size++;
}

void removeFront(SenderWindow &senderWindow) {
    senderWindow.isACKed[senderWindow.front] = false;
    senderWindow.startTransferTime[senderWindow.front] = -1;
    senderWindow.front = (senderWindow.front + 1) % senderWindow.maxSize;
    senderWindow.size--;

    if (senderWindow.size == 0) {
        senderWindow.front = -1;
        senderWindow.back = -1;
    }
}

int main(int argc, char *argv[]) {
    // receive input arguments
    if (argc < 6) {
        exit(EXIT_FAILURE);
    }

    string filename = argv[1];
    int windowsize = atoi(argv[2]);
    int buffersize = atoi(argv[3]);
    char *destination_ip = argv[4];
    int destination_port = atoi(argv[5]);

    // creating socket connection
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        exit(EXIT_FAILURE);
    }

    sockaddr_in receiverAddress;
    receiverAddress.sin_addr.s_addr = inet_addr(destination_ip);
    receiverAddress.sin_family = AF_INET;
    receiverAddress.sin_port = htons(destination_port);

    // creating thread for receiver response
    pthread_t recvResponseThread;
    if (pthread_create(&recvResponseThread, NULL, &recvResponse, NULL) < 0) {
        exit(EXIT_FAILURE);
    }

    // back to the main sender
    ifstream inputFile;
    inputFile.open(filename);
    bool endOfFile = false;

    while (true) {
        while (senderWindow.size <= senderWindow.maxSize / 2 && !endOfFile) {
            unsigned char character;
            if (inputFile.peek() == EOF) {
                character = FILE_ENDING;
                endOfFile = true;
            } else {
                inputFile >> noskipws >> character;
            }
            addToBack(character, senderWindow);
        }

        // iterate through senderWindow and send frame which have not been ACKed
        for (int i = senderWindow.front; i != senderWindow.back;
             i = (i + 1) % senderWindow.maxSize) {
            if (!senderWindow.isACKed[i]) {
                double deltaTime =
                    (double)(clock() - senderWindow.startTransferTime[i]) /
                    CLOCKS_PER_SEC * 1000;

                if (senderWindow.startTransferTime[i] == -1 ||
                    deltaTime > TIMEOUT) {

                    senderWindow.startTransferTime[i] = clock();
                    sendMessage(i, senderWindow.data[i], sock, receiverAddress,
                                sizeof(receiverAddress));
                }
            } else if (i == senderWindow.front && senderWindow.isACKed[i]) {
                removeFront(senderWindow);
            }
        }

        if (endOfFile && senderWindow.size == 0)
            break;
    }

    inputFile.close();

    return 0;
}
