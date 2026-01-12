#include "parser.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define CHUNK_SIZE 4096
#define MAX_HEADER_LENGTH 655353
// Using 6 because DELETE is the largest expected method
#define MAX_METHOD_LENTH 6

HttpRequest parse_http_request(int clientFD) {
    HttpRequestString requestString = {0};
    get_http_request_string(clientFD, &requestString);
    HttpRequest request;
    parse_startline(&requestString, &request);
    request.Host = get_http_request_entry(&requestString, "Host");
    request.UserAgent = get_http_request_entry(&requestString, "User-Agent");
    request.Accept = get_http_request_entry(&requestString, "Accept");
    request.AcceptLanguage = get_http_request_entry(&requestString, "Accept-Language");
    return request;
}

void get_http_request_string(int clientFD, HttpRequestString *requestString) {
    char temp[CHUNK_SIZE];
    char *headers = NULL;
    size_t headersLen = 0;

    while (!found_end_http_request(headers, headersLen)) {
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
    requestString->Header = headers;
    requestString->HeaderLength = headersLen;
}

bool found_end_http_request(char *httpRequest, size_t requestLength) {
    if (requestLength < 4) {
        return false;
    }
    // Check if the last 4 characters in httpRequest is \r\n\r\n
    // This signifies the end of the header
    if (httpRequest[requestLength - 1] == '\n' && httpRequest[requestLength - 3] == '\n' &&
        httpRequest[requestLength - 2] == '\r' && httpRequest[requestLength - 4] == '\r') {
        return true;
    }
    return false;
}

void parse_startline(HttpRequestString *httpRequestString, HttpRequest *request) {
    char *startLineStr = parse_startline_str(httpRequestString);
    set_starline_method(request, startLineStr);
    set_startline_request_target(request, startLineStr);
    set_startline_version(request, startLineStr);
}

char *parse_startline_str(HttpRequestString *httpRequestString) {
    int startLineLen = 0;
    // Find the end of the startLine
    while (startLineLen < httpRequestString->HeaderLength && startLineLen + 1 < httpRequestString->HeaderLength) {
        if (httpRequestString->Header[startLineLen] == '\r' && httpRequestString->Header[startLineLen + 1] == '\n') {
            break;
        }
        startLineLen++;
    }
    char *startLineStr = malloc(startLineLen + 1);
    strncpy(startLineStr, httpRequestString->Header, startLineLen);
    startLineStr[startLineLen] = '\0';
    return startLineStr;
}

void set_starline_method(HttpRequest *httpRequest, char *startLineStr) {
    char method[MAX_METHOD_LENTH];
    int methodLen = 0;
    for (int methodLen = 0; methodLen < MAX_METHOD_LENTH && startLineStr[methodLen] != ' '; ++methodLen) {
        method[methodLen] = startLineStr[methodLen];
    }
    if (strncmp(method, "GET", methodLen) == 0) {
        httpRequest->Method = GET;
    } else if (strncmp(method, "POST", methodLen) == 0) {
        httpRequest->Method = POST;
    } else if (strncmp(method, "PUT", methodLen) == 0) {
        httpRequest->Method = PUT;
    } else if (strncmp(method, "DELETE", methodLen) == 0) {
        httpRequest->Method = DELETE;
    }
}

void set_startline_request_target(HttpRequest *httpRequest, char *startLineStr) {
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
    httpRequest->RequestTarget = requestTarget;
}

void set_startline_version(HttpRequest *httpRequest, char *startLineStr) {
    size_t len = strlen(startLineStr);
    // -1 becuase don't want to point to '\0'
    char *end_ptr = startLineStr + len - 1;
    while (*end_ptr != ' ') {
        end_ptr--;
    }
    // Get rid of empty space at the begginning
    end_ptr++;
    httpRequest->Version = end_ptr;
}

char *get_http_request_entry(HttpRequestString *httpRequestString, char *entry) {
    // requestHeader will include "\r\n" before header and ":\0" after
    char requestHeader[strlen(entry) + 4];
    memset(requestHeader, 0, sizeof(requestHeader));
    strcat(requestHeader, "\r\n");
    strcat(requestHeader, entry);
    strcat(requestHeader, ":\0");

    char *resultStart = strstr(httpRequestString->Header, requestHeader);
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
        printf("There was a error parsing the value for header. Header: %s\n", entry);
        perror("Header parsing error");
    }
    size_t length = resultEnd - resultStart + 1;
    char *result = malloc(length);
    strncpy(result, resultStart, length - 1);
    result[length - 1] = '\0';
    return result;
}
