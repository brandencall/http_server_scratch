#ifndef HTTP_H
#define HTTP_H

#include "tcp.h"
#include <stdbool.h>
#include <stddef.h>
#include "http_models.h"

typedef struct {
    char *Header;
    size_t HeaderLength;
} HttpRequestString;

void start_http(int port);
void register_get(char *requestTarget, GetResponse (*callback)());
void handle_http_request(int clientFD);

void handle_get_request(HttpRequest *httpRequest, int clientFD);
GetHandler *get_handler(HttpRequest *httpRequest);

void set_content_length_response(HttpResponse *httpResponse, char *contentBody);
void set_status_response(HttpResponse *httpResponse, StatusCode *statusCode);
void set_date_response(HttpResponse *httpResponse);
void set_content_type_response(HttpResponse *httpResponse);

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
