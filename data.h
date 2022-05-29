#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>

#include "server.h"
#include "client.h"

#define PORT 5555
#define hostNameLength 50
#define messageLength 256
#define MAXMSG 4096

#ifndef data

typedef struct {
    bool SYN, ACK, FIN; // SYN, ACK, FIN
    int ACKnr, SEQ; // ACKnr (senders seqnr?), SEQ (sequence nr of current)
    int id, ws; // id (id of packet), length (window length/size)
    int crc; // crc (checksum)
    char * msgData; // data
}data;

int calc_Checksum(data * datapacket);
void calc_Sequence(data * datapacket);
int generate_Error(data * datapacket);
int generate_Number(int floor, int roof);
void zero_Packet(data * datapacket);

void packet_Write(int fileDescriptor, data * data, int length, struct sockaddr_in * hostInfo);
int packet_Read(int fileDescriptor, data * readData, struct sockaddr_in * hostInfo);

#endif