#include "http.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define CHUNK_SIZE 4096;
#define MAX_HEADER_LENGTH 65535;

bool found_header_end(char *headers, size_t headersLen) {

    if (headersLen < 4) {
        return false;
    }

    // Check if the last 4 characters in headers is \r\n\r\n
    // This signifies the end of the header
    if (headers[headersLen - 1] == '\n' && headers[headersLen - 3] == '\n' && headers[headersLen - 2] == '\r' &&
        headers[headersLen - 4] == '\r') {
        return true;
    }

    return false;
}

void handle_http(int clientFD) {
    char temp[4096];
    char *headers = NULL;
    size_t headersLen = 0;

    while (!found_header_end(headers, headersLen)) {
        ssize_t chunk = recv(clientFD, temp, sizeof(temp), 0);
        if (chunk <= 0) {
            perror("There was an error reading from the chunk\n");
        }
        headers = realloc(headers, headersLen + chunk);
        memcpy(headers + headersLen, temp, chunk);
        headersLen += chunk;

        if (65535 < headersLen) {
            perror("The request is too big. Need to reject it\n");
        }
    }

    printf("client said: %s\n", headers);
}
