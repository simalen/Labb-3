#include "data.h"

int calc_Checksum(data * datapacket) {
    // Ger checksumma.
}

void calc_Sequence(data * datapacket) {
    // Ger ett sekvensnummer.
    static int startNr = 0;
    startNr == 0 ? startNr = generate_Number(20, 2000000) : startNr++;
    datapacket->SEQ = startNr;
}

int generate_Error(data * datapacket) {
    // Ska göra egna fel i paketen osv för att simulera fel.
}

int generate_Number(int floor, int roof) {
    return (rand() % roof) + floor;
}

void zero_Packet(data * datapacket) {
    datapacket->SYN = false;
    datapacket->ACK = false;
    datapacket->FIN = false;

    datapacket->SEQ = 0;
    datapacket->crc = 0;
    datapacket->ws = 0;

    datapacket->id = generate_Number(10, 1000);
}

void packet_Write(int fileDescriptor, data * data, int length, struct sockaddr_in * hostInfo) {
    // Skickar paketet till fileDescriptor socket.
    int dataRetVal = sendto(fileDescriptor, data, length, 0, hostInfo, sizeof(*hostInfo));
    if(dataRetVal < 0) printf("\n(data) > Could not write data to socket.\n");
}

int packet_Read(int fileDescriptor, data * readData, struct sockaddr_in * hostInfo) {
    // Läser socket in i ett paket.
    int dataRetVal;
    unsigned nOfBytes = sizeof(*hostInfo);

    dataRetVal = recvfrom(fileDescriptor, readData, MAXMSG, 0, hostInfo, &nOfBytes);
    if(dataRetVal < 0) {
        printf("\n(data) > Could not read data from socket.\n");
        return -1;
    }
    return dataRetVal;
}