#include "tcp.h"
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define LISTEN_BACKLOG 128

int socket_init(int port) {
    int socketFD = socket(AF_INET6, SOCK_STREAM, 0);
    if (socketFD == -1) {
        printf("There was a problem creating the socket file descriptor\n");
    }
    struct sockaddr_in6 address;
    memset(&address, 0, sizeof(address));

    address.sin6_family = AF_INET6;
    address.sin6_port = htons(port);
    address.sin6_flowinfo = 0;
    address.sin6_addr = in6addr_any;
    address.sin6_scope_id = 0;

    int opt = 1;
    if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        printf("setsockopt FAILED\n");

    if (bind(socketFD, (struct sockaddr *)&address, sizeof(address)) == -1) {
        printf("Bind FAILED!\n");
    }

    if (listen(socketFD, LISTEN_BACKLOG) == -1) {
        printf("Listen FAILED!\n");
    }
    printf("Listening on port %d\n", port);

    return socketFD;
}

int wait_for_client(int socketFD) {
    struct sockaddr_un clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int clientFD = accept(socketFD, (struct sockaddr *)&clientAddr, &clientAddrSize);
    printf("client accepted\n");
    return clientFD;
}

void write_to_client(char *msg, int msgLen, int clientFD) { write(clientFD, msg, msgLen); }
