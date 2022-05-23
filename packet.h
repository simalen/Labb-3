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

#ifndef packet

typedef struct {
    bool SYN, ACK, FIN; // SYN, ACK, FIN
    int ACKnr, SEQ; // ACKnr (senders seqnr?), SEQ (sequence nr of current)
    int id, length; // id (id of packet), length (window length/size)
    int crc; // crc (checksum)
    char * msgData; // data
}packet;

#endif