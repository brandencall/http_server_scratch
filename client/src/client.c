#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define PORT 7777
#define SERVER_IP "127.0.0.1"
#define MAX_BUFFER_SIZE 1024

int main() {
    printf("Hello from client\n");

    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, SERVER_IP, &servAddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
    }

    int socketFD = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFD == -1) {
        printf("There was a problem creating the socket file descriptor\n");
    }

    if (connect(socketFD, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1) {
        printf("Failed to connect...\n");
    }

    char *msg = "Hi from Client\r\n\r\n";
    write(socketFD, msg, strlen(msg));

    close(socketFD);
    return 0;
}
