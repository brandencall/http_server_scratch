#include "http.h"
#include "parser.h"
#include "http_models.h"
#include "tcp.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define CHUNK_SIZE 4096
#define MAX_HEADER_LENGTH 655353
// Using 6 because DELETE is the largest expected method
#define MAX_METHOD_LENTH 6

GetHandler *getHead = NULL;
PostHandler *postHead = NULL;

void start_http(int port) {
    printf("Hello from server\n");

    int socketFD = socket_init(port);

    while (1) {
        int clientFD = wait_for_client(socketFD);
        handle_http_request(clientFD);

        close(clientFD);
        printf("closed client connection\n");
    }

    close(socketFD);
}

// May be able to combine both of the get and post register methods
void register_get(char *requestTarget, Response (*callback)()) {
    GetHandler *newHandler = (GetHandler *)malloc(sizeof(GetHandler));
    if (newHandler == NULL) {
        perror("Error allocating mememory for new get handler");
    }
    newHandler->RequestTarget = requestTarget;
    newHandler->callback = callback;
    newHandler->Next = getHead;
    getHead = newHandler;
}

void register_post(char *requestTarget, Response (*callback)(char *)) {
    PostHandler *newHandler = (PostHandler *)malloc(sizeof(PostHandler));
    if (newHandler == NULL) {
        perror("Error allocating mememory for new get handler");
    }
    newHandler->RequestTarget = requestTarget;
    newHandler->callback = callback;
    newHandler->Next = postHead;
    postHead = newHandler;
}

void handle_http_request(int clientFD) {
    HttpRequest request = parse_http_request(clientFD);
    if (request.Method == GET) {
        handle_get_request(&request, clientFD);
    } else if (request.Method == POST) {
        printf("Post method recognized\n");
        handle_post_request(&request, clientFD);
    }
}


// Can probably combine the get and post request handlers
void handle_get_request(HttpRequest *httpRequest, int clientFD) {
    HttpResponse httpResponse;
    httpResponse.Version = httpRequest->Version;
    GetHandler *getHandler = get_handler(httpRequest);
    if (getHandler == NULL) {
        printf("Request target not found. Request Target: %s\n", httpRequest->RequestTarget);
        set_status_response(&httpResponse, NOT_FOUND);
        httpResponse.ContentBody = "";
    } else {
        Response getResponse = getHandler->callback();
        httpResponse.ContentBody = getResponse.ContentBody;
        set_status_response(&httpResponse, getResponse.StatusCode);
    }
    set_content_length_response(&httpResponse);
    set_date_response(&httpResponse);
    set_content_type_response(&httpResponse);
    write_http_response(&httpResponse, clientFD);
}

void handle_post_request(HttpRequest *httpRequest, int clientFD){
    HttpResponse httpResponse;
    httpResponse.Version = httpRequest->Version;
    PostHandler *postHandler = post_handler(httpRequest);
    if (postHandler == NULL) {
        printf("Request target not found. Request Target: %s\n", httpRequest->RequestTarget);
        set_status_response(&httpResponse, NOT_FOUND);
        httpResponse.ContentBody = "";
    } else {
        Response postResponse = postHandler->callback(httpRequest->ContentBody);
        httpResponse.ContentBody = postResponse.ContentBody;
        set_status_response(&httpResponse, postResponse.StatusCode);
    }
    set_content_length_response(&httpResponse);
    set_date_response(&httpResponse);
    set_content_type_response(&httpResponse);
    write_http_response(&httpResponse, clientFD);
}

// May be able to also combine both `get_handler` and `post_handler`
GetHandler *get_handler(HttpRequest *httpRequest) {
    GetHandler *curr = getHead;
    while (curr != NULL) {
        // found the request target
        if (strcmp(curr->RequestTarget, httpRequest->RequestTarget) == 0){
            break;
        }
        curr = curr->Next;
    }
    return curr;
}

PostHandler *post_handler(HttpRequest *httpRequest) {
    PostHandler *curr = postHead;
    while (curr != NULL) {
        // found the request target
        if (strcmp(curr->RequestTarget, httpRequest->RequestTarget) == 0){
            break;
        }
        curr = curr->Next;
    }
    return curr;
}

void set_content_length_response(HttpResponse *httpResponse) {
    size_t contentLength = strlen(httpResponse->ContentBody);
    snprintf(httpResponse->ContentLength, sizeof(httpResponse->ContentLength), "%zu", contentLength);
}

void set_status_response(HttpResponse *httpResponse, StatusCode statusCode) {
    if (statusCode == OK) {
        httpResponse->Status = "200 OK";
    } else if (NOT_FOUND == statusCode) {
        httpResponse->Status = "404 Not Found";
    }
}

// TODO: Need to get the actual date and time and format it as below
void set_date_response(HttpResponse *httpResponse) { httpResponse->Date = "Fri, 09 Jan 2026 02:45:00 GMT"; }

// TODO: Need to have logic that sets the correct ContentType
void set_content_type_response(HttpResponse *httpResponse) { httpResponse->ContentType = "text/plain"; }

void print_header(HttpRequest *httpRequest) {
    printf("httpRequest.Method: %d\n", httpRequest->Method);
    printf("httpRequest.RequestTarget: %s\n", httpRequest->RequestTarget);
    printf("httpRequest.Version: %s\n", httpRequest->Version);
    printf("httpRequest.Host: %s\n", httpRequest->Host);
    printf("httpRequest.UserAgent: %s\n", httpRequest->UserAgent);
    printf("httpRequest.Accept: %s\n", httpRequest->Accept);
}
