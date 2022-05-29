#include "data.h"

void main(int argc, char *argv[]) {
    char hostName[hostNameLength];
    srand(time(NULL));
    if(argv[1] == NULL) {
        perror("Starting server..\n");
        main_server();
    }
    else {
        strncpy(hostName, argv[1], hostNameLength);
        hostName[hostNameLength - 1] = '\0';
        printf("Starting client at %s..\n", hostName);
        main_client(hostName);
    }
}