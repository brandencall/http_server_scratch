#ifndef HTTP_H
#define HTTP_H

#include "tcp.h"
#include <stdbool.h>
#include <stddef.h>
#include "http_models.h"

void start_http(int port);
void register_get(char *requestTarget, GetResponse (*callback)());
void handle_http_request(int clientFD);

void handle_get_request(HttpRequest *httpRequest, int clientFD);
GetHandler *get_handler(HttpRequest *httpRequest);

void set_content_length_response(HttpResponse *httpResponse, char *contentBody);
void set_status_response(HttpResponse *httpResponse, StatusCode *statusCode);
void set_date_response(HttpResponse *httpResponse);
void set_content_type_response(HttpResponse *httpResponse);

void print_header(HttpRequest *httpRequest);

#endif
