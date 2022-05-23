#include "packet.h"

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

int client_Connect(int * fileDescriptor, fd_set *activeFdSet, struct sockaddr_in * hostInfo) {

    /* Variables:
     * state - Used by switch (state-machine)
     * readFdSet - Used by select and FD_ISSET to see which sockets there is input on (We only use one)
     * running - Used in the loop around the switch-state
     * counter - Counter
     * ACK_NR - Used to temporary save the acknr in some cases
     * windowSize - A randomized number to represent the window size used later in sliding window
     * timer - Used by select to create timeouts
     * sSyn, rAck, sAck - Structs to read and write with between server/client */

    short state = 0;
    fd_set readFdSet;
    bool running = 1;
    int counter = 5;
    int ACK_NR = 0;
    int windowSize = (int) generate_Number(1, 5);
    struct timeval timer;

    packet sSyn;
    packet rAck;
    packet sAck;

    while(running) {
        switch(state) {

            // State 0, Klient skickar SYN till servern (Titta state machine).
            case 0:
                init_Packet(&sSyn); // Default värden på sSYN paketet.
                sSyn.SYN = true; // sSYN är ett SYN paket.
                sSyn.length = windowSize; // 
                calc_Sequence(&sSyn);

                while(counter > 0 && state == 0) {
                    packet_Write(*fileDescriptor, &sSyn, sizeof(sSyn), hostInfo);
                    printf("SYN sent with SEQ nr: %d", sSyn.seq);
                    timer.tv_sec = 0;
                    timer.tv_usec = 1;
                    
                    readFdSet = *activeFdSet;

                    // Tittar om readFdSet har input.
                    int fdSELECT = select(FD_SETSIZE, &readFdSet, NULL, NULL, &timer);
                    
                    if(t == -1) printf("Error");

                    if(FD_ISSET(*fileDescriptor, &readFdSet)) {
                        if(packet_Read(*fileDescriptor, &rAck, hostInfo) == 0) {
                            if(rAck.ACK == true && rAck.SYN == true && Ack.ACKnr == sSyn.seq) {
                                printf("Client received ACK+SYN");
                                ACK_NR = rAck.seq;
                                windowSize = rAck.length;
                                state = 1;
                            }
                            else {
                                printf("SYN + ACK not received from server");
                            }
                        }                        
                    }
                    else {
                        // Om SYN paketet är förlorat.
                        counter--;
                        state = 0;
                        if(counter == 0) exit(EXIT_FAILURE);
                    }
                }
                break;

            // State 1, Klient har fått SYN+ACK från servern (Titta state machine).
            case 1:
                init_Packet(&sAck);
                calc_Sequence(&sAck);
                sAck.ACK = true;
                sAck.ACKnr = ACK_NR;
        }
    }

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