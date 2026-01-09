#include "http.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define CHUNK_SIZE 4096
#define MAX_HEADER_LENGTH 655353
// Using 6 becuase DELETE is the largest expected method
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

        if (headersLen > MAX_HEADER_LENGTH) {
            perror("The request is too big. Need to reject it\n");
        }
    }
    headersLen++;
    headers = realloc(headers, headersLen);
    headers[headersLen - 1] = '\0';
    Header header = parse_header(headers, headersLen);
    print_header(&header);
    free(headers);
}

void print_header(Header *header) {
    printf("StartLine.Method: %d\n", header->StartLine.Method);
    printf("StartLine.RequestTarget: %s\n", header->StartLine.RequestTarget);
    printf("StartLine.Version: %s\n", header->StartLine.Version);
    printf("header.Host: %s\n", header->Host);
    printf("header.UserAgent: %s\n", header->UserAgent);
    printf("header.Accept: %s\n", header->Accept);
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

Header parse_header(char *headers, size_t headersLen) {
    Header header;
    header.StartLine = parse_startline(headers, headersLen);
    header.Host = get_header_entry(headers, "Host");
    header.UserAgent = get_header_entry(headers, "User-Agent");
    header.Accept = get_header_entry(headers, "Accept");
    header.AcceptLanguage = get_header_entry(headers, "Accept-Language");
    return header;
}

StartLine parse_startline(char *headers, size_t headersLen) {
    StartLine startLine;
    char *startLineStr = parse_startline_str(headers, headersLen);
    printf("Start Line: %s\n", startLineStr);

    set_starline_method(&startLine, startLineStr);
    set_startline_request_target(&startLine, startLineStr);
    set_startline_version(&startLine, startLineStr);

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
    char *startLineStr = malloc(startLineLen + 1);
    strncpy(startLineStr, headers, startLineLen);
    startLineStr[startLineLen] = '\0';
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

void set_startline_request_target(StartLine *startLine, char *startLineStr) {
    int startOfTarget = 0;
    while (startLineStr[startOfTarget] != '/') {
        startOfTarget++;
    }
    int endOfTarget = startOfTarget;
    while (startLineStr[endOfTarget] != ' ') {
        endOfTarget++;
    }
    int len = endOfTarget - startOfTarget;
    char *requestTarget = malloc(len + 1);
    for (int i = 0; i < len; ++i) {
        requestTarget[i] = startLineStr[startOfTarget + i];
    }
    requestTarget[len] = '\0';

    startLine->RequestTarget = requestTarget;
}

void set_startline_version(StartLine *startLine, char *startLineStr) {
    size_t len = strlen(startLineStr);
    // -1 becuase don't want to point to '\0'
    char *end_ptr = startLineStr + len - 1;

    while (*end_ptr != ' ') {
        end_ptr--;
    }
    // Get rid of empty space at the begginning
    end_ptr++;
    startLine->Version = end_ptr;
}

char *get_header_entry(char *fullHeader, char *header) {
    // requestHeader will include "\r\n" before header and ":\0" after
    char requestHeader[strlen(header) + 4];
    memset(requestHeader, 0, sizeof(requestHeader));
    strcat(requestHeader, "\r\n");
    strcat(requestHeader, header);
    strcat(requestHeader, ":\0");

    char *resultStart = strstr(fullHeader, requestHeader);
    // If the header is not present then just return an empty result
    if (resultStart == NULL) {
        char *emptyResult = malloc(1);
        emptyResult[0] = '\0';
        return emptyResult;
    }
    // Move the resultStart after Header so that we can just get the result of the header.
    while (*resultStart != ' ') {
        resultStart++;
    }
    // Get rid of empty space at the begginning
    resultStart++;
    char *endOfLine = "\r\n";
    char *resultEnd = strstr(resultStart, endOfLine);
    if (resultStart == NULL) {
        printf("There was a error parsing the value for header. Header: %s\n", header);
        perror("Header parsing error");
    }
    size_t length = resultEnd - resultStart + 1;
    char *result = malloc(length);
    strncpy(result, resultStart, length - 1);
    result[length - 1] = '\0';
    return result;
}
