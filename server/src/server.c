#include "http.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

char *test() {
    //char *msg = "Hi from testing";
    //int length = strlen(msg) + 1;
    //char *result = malloc(length);
    //strncpy(result, msg, length - 1);
    //result[length - 1] = '\0';
    //return result;
    return "Hi from testing";
}

int main() {
    register_get("/hi", test);
    start_http(7777);
    return 0;
}
