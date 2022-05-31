#include "data.h"

void clientInitSocketAdress(struct sockaddr_in *name, char *hostName, unsigned short int port) {
    struct hostent *hostInfo; /* Contains info about the host */
    /* Socket address format set to AF_INET for Internet use. */
    name->sin_family = AF_INET;     
    /* Set port number. The function htons converts from host byte order to network byte order.*/
    name->sin_port = htons(port);

    /* Get info about host. */
    hostInfo = gethostbyname(hostName); 
    if(hostInfo == NULL) {
        printf("\n(client.c) > Unknown host %s\n", hostName);
        exit(EXIT_FAILURE);
    }

    /* Fill in the host name into the sockaddr_in struct. */
    name->sin_addr = *(struct in_addr *)hostInfo->h_addr;
}

int client_Connect(int * fileDescriptor, fd_set *activeFdSet, struct sockaddr_in * hostInfo) {
    int state = 0, counter = 5, windowSize = (int) generate_Number(1, 5);
    int ACK_NR = 0;
    struct timeval timer;

    fd_set readFdSet;
    data clientSyn;
    data serverSynAck;
    data clientAck;

    printf("\n(client.c) > Attempting to CONNECT to server..\n");

    while(1) {
        switch(state) {

            // State 0, Klient skickar SYN till servern (Titta state machine).
            case 0:
                zero_Packet(&clientSyn); // Default värden på clientSyn paketet.
                clientSyn.SYN = true; // clientSyn är ett SYN paket.
                clientSyn.ws = windowSize;
                calc_Sequence(&clientSyn);

                while(counter > 0 && state == 0) {
                    packet_Write(*fileDescriptor, &clientSyn, sizeof(clientSyn), hostInfo);
                    printf("\n(client.c)(CON S0) > SYN packet sent with Sequence number: %d.\n", clientSyn.SEQ);
                    timer.tv_sec = 0;
                    timer.tv_usec = 1;
                    
                    readFdSet = *activeFdSet;

                    // Tittar om readFdSet har input.
                    int fdSelect = select(FD_SETSIZE, &readFdSet, NULL, NULL, &timer);
                    
                    if(fdSelect == -1) printf("\n(client.c)(CON S0) > Error selecting from readFdSet.\n");

                    if(FD_ISSET(*fileDescriptor, &readFdSet)) {
                        if(packet_Read(*fileDescriptor, &serverSynAck, hostInfo) == 0) {
                            if(serverSynAck.ACK == true && serverSynAck.SYN == true) {
                                printf("\n(client.c)(CON S0) > Client received ACK+SYN.\n");
                                windowSize = serverSynAck.ws;
                                ACK_NR = serverSynAck.SEQ;
                                state = 1;
                            }
                            else {
                                printf("\n(client.c)(CON S0) > SYN + ACK not received from server.\n");
                            }
                        }                        
                    }
                    else {
                        // Om SYN paketet är förlorat.
                        counter--;
                        state = 0;
                        if(counter == 0) {
                            printf("\n(client.c)(CON S0) > SYN + ACK not received in 5 tries.\n");
                            exit(EXIT_FAILURE);
                        }    
                    }
                }
                break;

            // State 1, Klient har fått SYN+ACK från servern (Titta state machine).
            case 1:
                zero_Packet(&clientAck);
                calc_Sequence(&clientAck);
                clientAck.ACK = true;
                clientAck.SEQR = ACK_NR;

                do {
                    printf("\n(client.c)(CON S1) > ACK sent with Sequence number: %d.\n", clientAck.SEQ);
                    packet_Write(*fileDescriptor, &clientAck, sizeof(clientAck), hostInfo);
                    // Titta ifall klienten får fler paket efter..
                    //struct timeval timer;
                    //timer.tv_sec = 1;
                    //timer.tv_usec = 0;
                    //
                    //readFdSet = *activeFdSet;
                    //int fdSelect = select(FD_SETSIZE, &readFdSet, NULL, NULL, &timer);
                    //
                    //if(fdSelect == -1) printf("\n(client.c)(CON S1) > Error selecting from readFdSet.\n");
                    //if(FD_ISSET(*fileDescriptor, &readFdSet)) {
                    //    data serverAck;
                    //    packet_Read(*fileDescriptor, &serverAck, hostInfo); 
                    //    if(serverAck.ACK == true && serverAck.SYN == true && serverAck.SEQR == clientSyn.SEQ) {
                    //        printf("\n(client.c)(CON S1) > Received duplicate ACK+SYN in final state.\n");
                    //    }
                    //}    
                    //else {
                    //    printf("\n(client.c)(CON S1) > Connection successful (CLIENT_CONNECT).\n"); 
                    //    return windowSize;                   
                    //}

                    printf("\n(client.c)(CON S1) > Connection successful (CLIENT_CONNECT).\n"); 
                    return windowSize; 
                }while(1);
        }
    }
    return windowSize;
}

int client_Disconnect(int * fileDescriptor, fd_set * activeFdSet, struct sockaddr_in * hostInfo) {
    int counter = 3;
    struct timeval timer;

    fd_set readFdSet;
    data clientFin;
    data serverFinAck;
    data clientAck;

    zero_Packet(&clientFin);
    zero_Packet(&clientAck);

    calc_Sequence(&clientFin);
    calc_Sequence(&clientAck);

    clientFin.FIN = true;
    clientAck.ACK = true;

    printf("\n(client.c)(DCON S0) > Attempting to DISCONNECT from server..\n");

    /* Klienten skickar ett FIN till servern.
     * Klienten väntar sedan på ett FIN+ACK från servern.
     * Klienten skickar ACK till servern innan den disconnectar
     */

    do {
        packet_Write(*fileDescriptor, &clientFin, sizeof(clientFin), hostInfo);
        printf("\n(client.c)(DCON S0) > FIN sent with Sequence number: %d\n", clientFin.SEQ);

        timer.tv_sec = 0;
        timer.tv_usec = 1;

        readFdSet = *activeFdSet;

        int fdSelect = select(FD_SETSIZE, &readFdSet, NULL, NULL, &timer);
                    
        if(fdSelect == -1) printf("\n(client.c)(DCON S0) > Error selecting from readFdSet.\n");

        if(FD_ISSET(*fileDescriptor, &readFdSet)) {
            if(packet_Read(*fileDescriptor, &serverFinAck, hostInfo) == 0) {
                if(serverFinAck.ACK == true && serverFinAck.FIN == true) {
                    printf("\n(client.c)(DCON S0) > FIN+ACK received from server. Sequence number: %d\n", serverFinAck.SEQ);
                    clientAck.SEQR = serverFinAck.SEQ;

                    packet_Write(*fileDescriptor, &clientAck, sizeof(clientAck), hostInfo);
                    printf("\n(client.c)(DCON S0) > ACK sent to server. Sequence number: %d\n", clientAck.SEQ);
                    printf("\n(client.c)(DCON S0) > Client disconnecting (CLIENT_DISCONNECT).\n");
                    return 0;
                }
                else {
                    printf("\n(client.c)(DCON S0) > No FIN + ACK received\n");
                }
            }
        }
        else {
            printf("\n(client.c)(DCON S0) > Timeout %d\n", 4-counter);
        }
        counter--;
    }while(counter >= 0);
    return -1;
}

void send_Slidingwindow() {

}

int main(int argc, char *argv[]) {
    if(argv[1] == NULL) {
        printf("\n(client.c)(main) > Usage ./client (hostname)..\n");
        exit(EXIT_FAILURE);
    }

    char hostName[hostNameLength];
    srand(time(NULL));
    strncpy(hostName, argv[1], hostNameLength);
    hostName[hostNameLength - 1] = '\0';
    printf("\n(client.c)(main) > Starting client at %s..\n", hostName);

    int fileDescriptor, windowSize = 0;
    struct sockaddr_in * hostInfo;
    hostInfo = malloc(sizeof(struct sockaddr_in));

    fd_set activeFdSet;

    fileDescriptor = socket(PF_INET, SOCK_DGRAM, 0);
    if(fileDescriptor < 0) {
        printf("\n(client.c)(main) > Could not create socket.\n");
        exit(EXIT_FAILURE);
    }
    clientInitSocketAdress(hostInfo, hostName, PORT);

    FD_ZERO(&activeFdSet);
    FD_SET(fileDescriptor, &activeFdSet);

    windowSize = client_Connect(&fileDescriptor, &activeFdSet, hostInfo);
    send_Slidingwindow(&fileDescriptor, &activeFdSet, hostInfo, &windowSize);
    client_Disconnect(&fileDescriptor, &activeFdSet, hostInfo);

    free(hostInfo);
    return 0;
}