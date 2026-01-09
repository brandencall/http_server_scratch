
#ifndef TCP_H
#define TCP_H

int socket_init();
int wait_for_client(int socketFD);
void write_to_client(char *msg, int msgLen, int clientFD);

#endif
