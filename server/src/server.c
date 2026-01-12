#include "http.h"
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

Response hi() {
    Response response = {OK, "testing from /hi"};
    return response;
}

Response hihi() {
    Response response = {OK, "testing from /hihi"};
    return response;
}

Response test(char *contentBody) {
    printf("Client said: %s\n", contentBody);
    Response response = {OK, ""};
    return response;
}

int main() {
    register_get("/hi", hi);
    register_get("/hihi", hihi);
    register_post("/test", test);
    start_http(7777);
    return 0;
}
