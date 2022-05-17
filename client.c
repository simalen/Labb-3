#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 5555
#define hostNameLength 50
#define messageLength 256

void error(char * msg, int line) {
    perror("\nLine %d: (client.c) > %s\n", line, msg);
}

void clientInitSocketAdress(struct sockaddr_in *name, char *hostName, unsigned short int port) {
    struct hostent *hostInfo; /* Contains info about the host */
    /* Socket address format set to AF_INET for Internet use. */
    name->sin_family = AF_INET;     
    /* Set port number. The function htons converts from host byte order to network byte order.*/
    name->sin_port = htons(port);

    /* Get info about host. */
    hostInfo = gethostbyname(hostName); 
    if(hostInfo == NULL) {
        fprintf(stderr, "initSocketAddress - Unknown host %s\n",hostName);
        exit(EXIT_FAILURE);
    }

    /* Fill in the host name into the sockaddr_in struct. */
    name->sin_addr = *(struct in_addr *)hostInfo->h_addr;
}

int client_Connect() {

}

int client_Disconnect() {
    
}

void send_Slidingwindow() {

}

void main(char * address) {
    int fileDescriptor, windowSize = 0;
    struct sockaddr_in hostInfo;
    fd_set activeFdSet;

    fileDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
    if(fileDescriptor < 0) {
        error("Could not create a socket", 58);
        exit(EXIT_FAILURE);
    }
    clientInitSocketAdress(&hostInfo, address, PORT);

    FD_ZERO(&activeFdSet);
    FD_SET(fileDescriptor, &activeFdSet);

    windowSize = client_Connect(&fileDescriptor, &activeFdSet, &hostInfo);
    send_Slidingwindow(&fileDescriptor, &activeFdSet, &hostInfo, &windowSize);
    client_Disconnect(&fileDescriptor, &activeFdSet, &hostInfo, &windowSize);
}