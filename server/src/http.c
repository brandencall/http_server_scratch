#include "http.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define CHUNK_SIZE 4096
#define MAX_HEADER_LENGTH 655353
// Using 6 becuase DELETE is the largest expect method
#define MAX_METHOD_LENTH 6

void handle_http(int clientFD) {
    char temp[CHUNK_SIZE];
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

        if (MAX_HEADER_LENGTH < headersLen) {
            perror("The request is too big. Need to reject it\n");
        }
    }

    printf("client said: %s\n", headers);
    parse_header(headers, headersLen);
}

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

void parse_header(char *headers, size_t headersLen) {
    StartLine startLine = parse_startline(headers, headersLen);
    printf("startLine method parsed: %d\n", startLine.Method);
}

// TODO: Implement parsing the HTTP version
StartLine parse_startline(char *headers, size_t headersLen) {
    StartLine startLine;
    char *startLineStr = parse_startline_str(headers, headersLen);
    printf("Start Line: %s\n", startLineStr);

    set_starline_method(&startLine, startLineStr);
    set_startline_request_target(&startLine, startLineStr);

    free(startLineStr);
    return startLine;
}

char *parse_startline_str(char *headers, size_t headersLen) {
    int startLineLen = 0;
    // Find the end of the startLine
    while (startLineLen < headersLen && startLineLen + 1 < headersLen) {
        if (headers[startLineLen] == '\r' && headers[startLineLen + 1] == '\n') {
            break;
        }
        startLineLen++;
    }
    char *startLineStr = (char *)malloc(startLineLen + 1);
    strcpy(startLineStr, headers);
    startLineStr[startLineLen + 1] = '\0';
    return startLineStr;
}

void set_starline_method(StartLine *startLine, char *startLineStr) {
    char method[MAX_METHOD_LENTH];
    int methodLen = 0;
    for (int methodLen = 0; methodLen < MAX_METHOD_LENTH && startLineStr[methodLen] != ' '; ++methodLen) {
        method[methodLen] = startLineStr[methodLen];
    }

    if (strncmp(method, "GET", methodLen) == 0) {
        startLine->Method = GET;
    } else if (strncmp(method, "POST", methodLen) == 0) {
        startLine->Method = POST;
    } else if (strncmp(method, "PUT", methodLen) == 0) {
        startLine->Method = PUT;
    } else if (strncmp(method, "DELETE", methodLen) == 0) {
        startLine->Method = DELETE;
    }
}

void set_startline_request_target(StartLine *startLine, char *startLineStr){
    int startOfTarget = 0;
    while (startLineStr[startOfTarget] != '/'){
        startOfTarget++;
    }
    int endOfTarget = startOfTarget;
    while (startLineStr[endOfTarget] != ' ') {
        endOfTarget++;
    }
    int len = endOfTarget - startOfTarget;
    char requestTarget[len + 1];
    for (int i = 0; i < len; ++i) {
        requestTarget[i] = startLineStr[startOfTarget + i];
    }
    requestTarget[len] = '\0';

    startLine->RequestTarget = requestTarget;
}
