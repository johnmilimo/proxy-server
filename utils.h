typedef struct Header {
    char *name;
    char *value;
    struct Header *next;
} Header;

typedef struct Request {
    char *method;
    char *uri;
    struct Header *headers;
    char *body;
} Request;

char *response;

Request parse_http_request(char *raw);

int handle_200();

int handle_404();
