#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Request parse_http_request(char *raw) {
    printf( "1. ORIGINAL REQUEST: \n\n%s\n", raw );

    Request req;
    sscanf(raw, "%s %s", req.method, req.uri);

    return req;
}

int handle_200() {
    /*
    NOTE:
        1. Content-Length - in the header. It only includes 
            the length of the body
        2. response_length - used by send(). It is the total 
            length of both header and body
    */

    // sample response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 20\n\nkijana fupi nono round - Mulmulwas";

    char *message;
    char *content_length;
    char *content_type;
    char *http_status;

    message = "Good judgment comes from experience, and a lot of that comes from bad judgment.\n";
    content_type = "Content-Type: text/plain\n";
    http_status = "HTTP/1.1 200 OK\n";

    content_length  = malloc(sizeof(long)+ strlen("Content-Length: ") + 2);
    sprintf(content_length, "Content-Length: %lu\n\n", strlen(message));

    response = malloc(strlen(http_status) + strlen(content_type) + strlen(content_length) + strlen(message) + 4);
    strcat(response, http_status);
    strcat(response, content_type);
    strcat(response, content_length);
    strcat(response, message);

    free(content_length);

    return 1;
}


int handle_404() {

    char *content_length;
    char *content_type;
    char *http_status;

    content_type = "Content-Type: text/plain\n";
    http_status = "HTTP/1.1 404 Not Found\n";
    content_length = "Content-Length: 0\n\n";

    response = malloc(strlen(http_status) + strlen(content_type) + strlen(content_length) + 3);
    strcat(response, http_status);
    strcat(response, content_type);
    strcat(response, content_length);

    return 1;
}
