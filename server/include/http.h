#ifndef HTTP_H
#define HTTP_H

#include <stdbool.h>
#include <stddef.h>

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

void handle_http(int clientFD);
bool found_header_end(char *headers, size_t headersLen);
Header parse_header(char *headers, size_t headersLen);
StartLine parse_startline(char *headers, size_t headersLen);
char *parse_startline_str(char *headers, size_t headersLen);
void set_starline_method(StartLine *startLine, char *startLineStr);
void set_startline_request_target(StartLine *startLine, char *startLineStr);
void set_startline_version(StartLine *startLine, char *startLineStr);
void print_header(Header *header);

char *parse_host(char *headers);
char *get_header_entry(char *fullHeader, char *header);

#endif
