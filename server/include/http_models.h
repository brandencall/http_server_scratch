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
} GetResponse;

typedef struct GetHandler {
    char *RequestTarget;
    GetResponse (*callback)();
    struct GetHandler *Next;
} GetHandler;

#endif
