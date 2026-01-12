#include "tcp.h"
#include "http_models.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
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

void write_http_response(HttpResponse *httpResponse, int clientFD) {
    char *version = get_version_str(httpResponse);
    char *status = get_status_str(httpResponse);
    char *date = get_entry_str(httpResponse->Date, "Date: ");
    char *contentType = get_entry_str(httpResponse->ContentType, "Content-Type: ");
    char *contentLength = get_entry_str(httpResponse->ContentLength, "Content-Length: ");
    char *newLine = "\r\n";
    int totalLength = strlen(version) + strlen(status) + strlen(date) + strlen(contentType) + strlen(contentLength) +
                      strlen(newLine) + strlen(httpResponse->ContentBody) + 1;
    char returnMsg[totalLength];
    memset(returnMsg, 0, sizeof(returnMsg));
    strcat(returnMsg, version);
    strcat(returnMsg, status);
    strcat(returnMsg, date);
    strcat(returnMsg, contentType);
    strcat(returnMsg, contentLength);
    strcat(returnMsg, newLine);
    strcat(returnMsg, httpResponse->ContentBody);
    write(clientFD, returnMsg, totalLength);

    // Clean up allocated memory
    free(version);
    free(status);
    free(date);
    free(contentType);
    free(contentLength);
}

char *get_version_str(HttpResponse *httpResponse) {
    char *space = " ";
    // Length of Version + space + null terminator
    int resultLen = strlen(httpResponse->Version) + strlen(space) + 1;
    char *result = calloc(resultLen, sizeof(char));
    strcat(result, httpResponse->Version);
    strcat(result, space);
    return result;
}

char *get_status_str(HttpResponse *httpResponse) {
    char *newLine = "\r\n";
    // Length of status + newline + null terminator
    int resultLen = strlen(httpResponse->Status) + strlen(newLine) + 1;
    char *result = calloc(resultLen, sizeof(char));
    strcat(result, httpResponse->Status);
    strcat(result, newLine);
    return result;
}

char *get_entry_str(char *entry, char *tag) {
    char *newLine = "\r\n";
    // Length of status + newline + null terminator
    int resultLen = strlen(entry) + strlen(tag) + strlen(newLine) + 1;
    char *result = calloc(resultLen, sizeof(char));
    strcat(result, tag);
    strcat(result, entry);
    strcat(result, newLine);
    return result;
}
