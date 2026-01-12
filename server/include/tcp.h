
#ifndef TCP_H
#define TCP_H

#include "http_models.h"

int socket_init(int port);
int wait_for_client(int socketFD);
void write_http_response(HttpResponse *httpResponse, int clientFD);

// May want to move these to the parsers at some point
char *get_version_str(HttpResponse *httpResponse);
char *get_status_str(HttpResponse *httpResponse);
char *get_entry_str(char *entry, char *tag);

#endif
