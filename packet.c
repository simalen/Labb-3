#include "packet.h"

static {
    srand(time(NULL));
}

int calc_Checksum(packet * datapacket) {
    // Ger checksumma.
}

void calc_Sequence(packet * datapacket) {
    // Ger ett sekvensnummer.
}

int generate_Error() {
    // Ska göra egna fel i paketen osv för att simulera fel.
}

int generate_Number(int floor, int roof) {
    return (rand() % roof) + floor;
}

void init_Packet(packet * datapacket) {
    datapacket->SYN = false;
    datapacket->ACK = false;
    datapacket->FIN = false;

    datapacket->seq = 0;
    datapacket->crc = 0;
    datapacket->windowSize = 0;
    datapacket->flags = 0;

    datapacket->id = generate_Number(10, 1000);

    // malloc() packet data pointer?

}

void packet_Write(int fileDescriptor, packet * data, int length, struct sockaddr_in * hostInfo) {
    // Skickar paketet till fileDescriptor socket.
}

void packet_Read(int fileDescriptor, packet * readData, struct sockaddr_in * hostInfo) {
    // Läser socket in i ett paket.
}