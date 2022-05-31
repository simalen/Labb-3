#include "data.h"

int makeSocket(unsigned short int port) {
    int sock;
    struct sockaddr_in name;
    
    /* Create a socket (DGRAM = UDP). */
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock < 0) {
        printf("\n(server.c) > Could not create a socket\n");
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
        printf("\n(server.c) > Could not bind a name to the socket\n");
        exit(EXIT_FAILURE);
    }
    return(sock);
}

int n;

int server_Connect(int *fileDescriptor, fd_set *activeFdSet, struct sockaddr_in *hostInfo) {
    data write, read;
    int state = 0;
    int counter = 0;
    int windowSize = (int) generate_Number(3, 20);
    struct timeval timer;
    fd_set readFdSet;

    printf("\n(server.c)(CON S0) > Attempting to CONNECT to client..\n");

    do {
        switch (state) {
            // Waiting for SYN from client..
            case 0:
                readFdSet = *activeFdSet;
                if(select(FD_SETSIZE, &readFdSet, NULL, NULL, &timer) < 0) printf("\n(server.c)(CON S0) > Error selecting from readFdSet.\n");
                if(FD_ISSET(*fileDescriptor, &readFdSet)) {
                    printf("\n(server.c)(CON S0) > Receiving input on socket, reading socket..\n");
                    if(packet_Read(*fileDescriptor, &read, hostInfo) == 0) {
                        if(read.SYN == true) {
                            printf("\n(server.c)(CON S0) > SYN received. Sequence number: %d\n", read.SEQ);
                            if(read.ws < windowSize) windowSize = read.ws;
                            state = 1;
                        }
                    }
                }
                break;
            // Send SYN+ACK to client..
            case 1:
                zero_Packet(&write);
                write.ACK = true; 
                write.SYN = true;
                write.ws = windowSize;
                calc_Sequence(&write);
                write.SEQR = read.SEQ;

                do {
                    packet_Write(*fileDescriptor, &write, sizeof(write), hostInfo);
                    printf("\n(server.c)(CON S1) > ACK+SYN sent on %d with Sequence number: %d.\n", write.SEQR, write.SEQ);

                    timer.tv_sec = 0;
                    timer.tv_usec = 300000;
                    readFdSet = *activeFdSet;

                    if(select(FD_SETSIZE, &readFdSet, NULL, NULL, &timer) < 0) printf("\n(server.c)(CON S1) > Error selecting from readFdSet.\n");
                    if(FD_ISSET(*fileDescriptor, &readFdSet)) {
                        // Return -1 if checksum doesnt match in packet_read.
                        if(packet_Read(*fileDescriptor, &read, hostInfo) == -1) {
                            printf("\n(server.c)(CON S1) > Wrong checksum received\n");
                            zero_Packet(&read);
                        }
                        if(read.ACK == true && read.SEQR == write.SEQ) {
                            printf("\n(server.c)(CON S1) > Final ACK received with Sequence number: %d.\n", read.SEQ);
                            // Handshake success.
                            printf("\n(server.c)(CON S1) > Connection successful (SERVER_CONNECT).\n");
                            n = read.SEQ;
                            return windowSize;
                        }
                        else {
                            printf("\n(server.c)(CON S1) > ACK not received. Attempt number %d.\n", counter+1);
                            counter++;
                        }
                    }
                    else {
                        printf("\n(server.c)(CON S1) > Timeout.\n");
                    }
                    if(counter > 3) {
                        exit(EXIT_FAILURE);
                    }     

                }while(state && counter <= 3);
                break;    
        }
    }while (1);
    return windowSize;
}

void server_Disconnect(int *fileDescriptor, fd_set *activeFdSet, struct sockaddr_in *hostInfo) {
    data write, read;
    int counter = 0;
    int state = 0;
    struct timeval timer;
    fd_set readFdSet;

    zero_Packet(&write);
    zero_Packet(&read);
    calc_Sequence(&write);
    write.ACK = true;
    write.FIN = true;
    write.SEQR = n;

    printf("\n(server.c)(DCON S0) > Attempting to DISCONNECT from client..\n");

    do {
        // Receive FIN from client.
        switch(state) {
            case 0:
                readFdSet = *activeFdSet;
                if(select(FD_SETSIZE, &readFdSet, NULL, NULL, &timer) < 0) printf("\n(server.c)(CON S0) > Error selecting from readFdSet.\n");
                if(FD_ISSET(*fileDescriptor, &readFdSet)) {
                    printf("\n(server.c)(DCON S0) > Receiving input on socket, reading socket..\n");
                    if(packet_Read(*fileDescriptor, &read, hostInfo) == 0) {
                        if(read.FIN == true) {
                            read.SEQR = write.SEQ;
                            printf("\n(server.c)(DCON S0) > FIN received. Sequence number: %d\n", read.SEQ);
                            state = 1;
                        }
                    }
                }
            // Send FIN+ACK to client and wait for ACK/Timeout.
            case 1:
                packet_Write(*fileDescriptor, &write, sizeof(write), hostInfo);
                printf("\n(server.c)(DCON S1) > FIN+ACK sent to the client with Sequence number: %d\n", write.SEQ);

                timer.tv_sec = 1;
                timer.tv_usec = 0;

                readFdSet = *activeFdSet;

                if(select(FD_SETSIZE, &readFdSet, NULL, NULL, &timer) < 0) printf("\n(server.c)(DCON S1) > Error selecting from readFdSet.\n");
                if(FD_ISSET(*fileDescriptor, &readFdSet)) {
                    if(packet_Read(*fileDescriptor, &read, hostInfo) == 0) {
                        if(read.ACK == true && read.SEQR == write.SEQ) {
                            printf("\n(server.c)(DCON S1) > ACK received, Sequence number: %d.\n", read.SEQ);
                            printf("\n(server.c)(DCON S1) > Server disconnecting (SERVER_DISCONNECT).\n");
                            return;
                        }
                    }
                }
                else {
                    counter++;
                    printf("\n(server.c) > Timeout %d.\n", counter+1);
                }
        }
    }while(counter <= 3);
}

void receive_Slidingwindow(int *fileDescriptor, fd_set *activeFdSet, struct sockaddr_in *hostInfo, int windowSize) {
    //data write, read;
    //int windowPlace, currentPlace, oldestPlace;

    //data * window = malloc(windowSize * sizeof(data));
    //fd_set readFdSet;

    //zero_Packet(&read);
    //zero_Packet(&write);


}

int main(void) {
    srand(time(NULL));
    printf("\n(server.c) > Starting server..\n");
    int fileDescriptor, windowSize = 0;
    struct sockaddr_in hostInfo;
    fd_set activeFdSet;
    
    fileDescriptor = makeSocket(PORT);

    FD_ZERO(&activeFdSet);
    FD_SET(fileDescriptor, &activeFdSet);

    printf("\n(server.c) > Waiting for connection...\n");

    windowSize = server_Connect(&fileDescriptor, &activeFdSet, &hostInfo);
    receive_Slidingwindow(&fileDescriptor, &activeFdSet, &hostInfo, windowSize);
    server_Disconnect(&fileDescriptor, &activeFdSet, &hostInfo);

    return 0;
}   