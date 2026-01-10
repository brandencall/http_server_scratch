#ifndef HTTP_H
#define HTTP_H

#include <stdbool.h>
#include <stddef.h>
#include "tcp.h"

typedef struct {
    char *Header;
    size_t HeaderLength;
} HeaderString;

typedef enum { GET, POST, PUT, DELETE } Method;

typedef struct {
    Method Method;
    char *RequestTarget;
    char *Version;
} StartLine;

// Need to fill out the rest of the request headers
// ref: https://developer.mozilla.org/en-US/docs/Glossary/Request_header
typedef struct {
    StartLine StartLine;
    char *Host;
    char *UserAgent;
    char *Accept;
    char *AcceptLanguage;
} Header;

void start_http(int port);

void handle_http(int clientFD);
HeaderString read_header(int clientFD);
Header parse_header(HeaderString *headerString);
void handle_get_request(Header *header, int clientFD);
bool found_header_end(char *headers, size_t headersLen);
StartLine parse_startline(HeaderString *headerString);
char *parse_startline_str(HeaderString *headerString);
void set_starline_method(StartLine *startLine, char *startLineStr);
void set_startline_request_target(StartLine *startLine, char *startLineStr);
void set_startline_version(StartLine *startLine, char *startLineStr);
void print_header(Header *header);

char *parse_host(char *headers);
char *get_header_entry(HeaderString *headerString, char *header);

#endif
