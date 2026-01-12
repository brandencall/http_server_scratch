
#ifndef TCP_H
#define TCP_H

#include "http_models.h"

int socket_init(int port);
int wait_for_client(int socketFD);
void write_http_response(HttpResponse *httpResponse, int clientFD);

char *get_version_str(HttpResponse *httpResponse);
char *get_status_str(HttpResponse *httpResponse);
char *get_entry_str(char *entry, char *tag);

#endif
