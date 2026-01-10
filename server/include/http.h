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
    char *Header;
    size_t HeaderLength;
} HttpRequestString;

typedef struct {
    // Method, RequestTarget, Version are the Start Line of the HTTP Request
    Method Method;
    char *RequestTarget;
    char *Version;
    char *Host;
    char *UserAgent;
    char *Accept;
    char *AcceptLanguage;
} HttpRequest;

typedef struct {
    char *RequestTarget;
    char *(*callback)();
} GetHandler;


void start_http(int port);
void register_get(char *requestTarget, char *(*callback)());
void handle_http_request(int clientFD);

void handle_get_request(HttpRequest *httpRequest, int clientFD);

HttpRequestString get_http_request_string(int clientFD);
bool found_end_http_request(char *httpRequest, size_t requestLength);

HttpRequest parse_http_request(HttpRequestString *httpRequestString);

void parse_startline(HttpRequestString *httpRequestString, HttpRequest *request);
char *parse_startline_str(HttpRequestString *httpRequestString);
void set_starline_method(HttpRequest *httpRequest, char *startLineStr);
void set_startline_request_target(HttpRequest *httpRequest, char *startLineStr);
void set_startline_version(HttpRequest *httpRequest, char *startLineStr);

char *get_http_request_entry(HttpRequestString *httpRequestString, char *entry);

void print_header(HttpRequest *httpRequest);

#endif
