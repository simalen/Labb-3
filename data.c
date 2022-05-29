#include "data.h"

int calc_Checksum(data * datapacket) {
    // Ger checksumma.
}

void calc_Sequence(data * datapacket) {
    // Ger ett sekvensnummer.
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

    datapacket->seq = 0;
    datapacket->crc = 0;
    datapacket->windowSize = 0;
    datapacket->flags = 0;

    datapacket->id = generate_Number(10, 1000);
}

void packet_Write(int fileDescriptor, data * data, int length, struct sockaddr_in * hostInfo) {
    // Skickar paketet till fileDescriptor socket.
}

void packet_Read(int fileDescriptor, data * readData, struct sockaddr_in * hostInfo) {
    // Läser socket in i ett paket.
}