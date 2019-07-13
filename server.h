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

Request parse_http_request(char *raw);

int handle_404();

int handle_random_number();
