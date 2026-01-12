#include "http.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

GetResponse hi() {
    GetResponse response = {OK, "testing from /hi"};
    return response;
}

GetResponse hihi() {
    GetResponse response = {OK, "testing from /hihi"};
    return response;
}

int main() {
    register_get("/hi", hi);
    register_get("/hihi", hihi);
    start_http(7777);
    return 0;
}
