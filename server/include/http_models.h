#ifndef HTTP_MODELS_H
#define HTTP_MODELS_H

typedef enum { GET, POST, PUT, DELETE } Method;
typedef enum { OK = 200, NOT_FOUND = 404 } StatusCode;

typedef struct {
    // Method, RequestTarget, Version are the Start Line of the HTTP Request
    Method Method;
    char *RequestTarget;
    char *Version;
    char *Host;
    char *UserAgent;
    char *Accept;
    char *AcceptLanguage;
    char *ContentBody;
    int ContentLength;
} HttpRequest;

typedef struct {
    char *Status;
    char *Version;
    char *Date;
    char *ContentType;
    char ContentLength[20];
    char *ContentBody;
} HttpResponse;

typedef struct {
    StatusCode StatusCode;
    char *ContentBody;
} Response;

typedef struct GetHandler {
    char *RequestTarget;
    Response (*callback)();
    struct GetHandler *Next;
} GetHandler;

typedef struct PostHandler{
    char *RequestTarget;
    Response (*callback)(char *contentBody);
    struct PostHandler *Next;
} PostHandler;

#endif
