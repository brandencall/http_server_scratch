#include "http.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

char *test() {
    return "Hi from testing";
}

int main() {
    register_get("/hi", test);
    start_http(7777);
    return 0;
}
