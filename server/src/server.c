#include "http.h"
#include "tcp.h"
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main() {
    printf("Hello from server\n");

    int socketFD = socket_init();

    while (1) {
        int clientFD = wait_for_client(socketFD);
        handle_http(clientFD);

        close(clientFD);
        printf("closed client connection\n");
    }

    close(socketFD);

    return 0;
}
