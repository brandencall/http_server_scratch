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

void handle_http(int clientFD);
bool found_header_end(char *headers, size_t headersLen);
void parse_header(char *headers, size_t headersLen);
StartLine parse_startline(char *headers, size_t headersLen);
char *parse_startline_str(char *headers, size_t headersLen);
void set_starline_method(StartLine *startLine, char *startLineStr);

#endif
