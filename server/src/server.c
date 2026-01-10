#include "http.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

GetResponse test() {
    GetResponse response = { OK, "Is this test working" };
    return response;
}

int main() {
    register_get("/hi", test);
    start_http(7777);
    return 0;
}
