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

void error(char * msg, int line) {
    perror("\nLine %d: (server.c) > %s\n", line, msg);
}

int makeSocket(unsigned short int port) {
    int sock;
    struct sockaddr_in name;
    
    /* Create a socket (DGRAM = UDP). */
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock < 0) {
        error("Could not create a socket", 22);
        exit(EXIT_FAILURE);
    }
    /* Give the socket a name. */
    /* Socket address format set to AF_INET for Internet use. */
    name.sin_family = AF_INET;
    /* Set port number. The function htons converts from host byte order to network byte order.*/
    name.sin_port = htons(port);
    /* Set the Internet address of the host the function is called from. */
    /* The function htonl converts INADDR_ANY from host byte order to network byte order. */
    /* (htonl does the same thing as htons but the former converts a long integer whereas
    * htons converts a short.) 
    */
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    /* Assign an address to the socket by calling bind. */
    if(bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
        error("Could not bind a name to the socket", 42);
        exit(EXIT_FAILURE);
    }
    return(sock);
}

void server_Disconnect(int *fileDescriptor, fd_set *activeFdSet, struct sockaddr_in *hostInfo) {
    
}

int handShake(int *fileDescriptor, fd_set *activeFdSet, struct sockaddr_in *hostInfo) {

}

void receive_Slidingwindow(int *fileDescriptor, fd_set *activeFdSet, struct sockaddr_in *hostInfo, int windowSize) {

}

void main(void) {
    int fileDescriptor, windowSize = 0;
    struct sockaddr_in hostInfo;
    fd_set activeFdSet;
    
    fileDescriptor = makeSocket(PORT);

    FD_ZERO(&activeFdSet);
    FD_SET(fileDescriptor, &activeFdSet);

    error("Waiting for connection...", 70);

    windowSize = handshake(&fileDescriptor, &activeFdSet, &hostInfo);
    receive_Slidingwindow(&fileDescriptor, &activeFdSet, &hostInfo, &windowSize);
    server_Disconnect(&fileDescriptor, &activeFdSet, &hostInfo, &windowSize);
}   