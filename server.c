//
// Created by John Milimo on 2019-04-23.
//
/**
References
----------
1. https://github.com/LambdaSchool/C-Web-Server
2. https://github.com/kklis/proxy/blob/master/proxy.c

**/

#include "utils.h"
#include "proxy.h"

#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

void server_run();
void handle_client(int client_sock);
int create_connection();
void serve_client_request(int client_sock);
int check_ipversion(const char * address);
int create_server_socket();

int connections_processed = 0;
int server_sock, client_sock, remote_sock;
char const *remote_host;
char const *remote_port;

typedef enum {TRUE = 1, FALSE = 0} bool;
#define SERVER_PORT 8088  //Where the clients can reach at
#define PROXY TRUE

/*
 * Socket -> Bind -> listen -> accept -> read -> write -> read -> close
 */
int main(int argc, char const *argv[]) {

    remote_host = argv[1];
    remote_port = argv[2];

    if(create_server_socket() == 0) {
        return 0;
    }
    
    /*
     * Accept connections
     *
     * int accept(int server_sock, struct sockaddr *addr, socklen_t *addrlen)
     *
     * The accept system call grabs the first connection request on the queue of pending connections (set up in listen) and
     * creates a new socket for that connection.
     * The original socket that was set up for listening is used only for accepting connections,
     * not for exchanging data. By default, socket operations are synchronous, or blocking, and accept will block
     * until a connection is present on the queue.
     */
    server_run();

}


int create_server_socket() {

    /* 1. Create/Setup a socket. This returns a file descriptor.
    * int socket(int domain, int type, int protocol);
    *
    * domain
    *   - AF_INET for IPv4
    *   - AF_INET6 for IPv6
    * type
    *   - SOCK_STREAM for TCP
    *   - SOCK_DGRAM for UDP
    * protocol
    *   - 0
    */
    if((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Cannot create socket");
        return 0;
    }


    /* 2. Identify/Name the socket
     * When we talk about naming a socket, we are talking about
     * assigning a transport address to the socket (a port number in IP networking).
     * In sockets, this operation is called binding an address and the bind system call is used for this.
     *  - Where is the remote machine ( IP address, hostname )
     *  - What service gets the data ( port )
     */


    // this structure is defined in the header netinet/in.h
    /*
     * struct sockaddr_in
     *   {
     *       __uint8_t         sin_len;
     *       sa_family_t       sin_family;
     *       in_port_t         sin_port;
     *       struct in_addr    sin_addr;
     *       char              sin_zero[8];
     *   };
     */
    struct sockaddr_in address, caddress;

    // memset() is used to fill a block of memory with a particular value. It is in the <string.h> header
    /* htonl converts a long integer (e.g. address) to a network representation */
    /* htons converts a short integer (e.g. port) to a network representation */

    memset((char *)&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(SERVER_PORT);

    if((bind(server_sock, (struct sockaddr *) &address, sizeof(address))) < 0) {
        perror("Bind failed");
        return 0;
    }


    /*
     * 3. Wait/Listen for an incoming connection
     *
     * int listen(int socket, int backlog);
     *
     * - socket is the fd we created
     * - backlog is number of pending connections to queue before they are refused
     */

    if (listen(server_sock, 5) < 0) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    return 1;
}

/*
For reference regarding fork(), see:
 - http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/fork/create.html
 - http://man7.org/linux/man-pages/man2/fork.2.html
*/

void server_run() {

    struct sockaddr_storage client_addr;
    socklen_t addrlen = sizeof(client_addr);

    while (TRUE) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addrlen);
        if (fork() == 0) { // handle client connection in a separate process
            close(server_sock);
            handle_client(client_sock);
            exit(0);
        } else
            connections_processed++;
        
        close(client_sock);
    }
}


/* Handle client connection */
void handle_client(int client_sock)
{
    if(!PROXY) {
        serve_client_request(client_sock);
    } else {

        if ((remote_sock = create_connection()) < 0) {
            puts("Cannot connect to host");
            close(remote_sock);
            close(client_sock);
        } else {
            if (fork() == 0) { // a process forwarding data from client to remote socket
                forward_data(client_sock, remote_sock);
                exit(0);
            }

            if (fork() == 0) { // a process forwarding data from remote socket to client
                forward_data(remote_sock, client_sock);
                exit(0);
            }
        }
    }
}

void serve_client_request(int client_sock) {

        // 5. Send and Receive messages
        char message_buffer[30000] = {0};
        long valread = read(client_sock, message_buffer, 30000);

        if(strncmp(message_buffer, "GET / ", 6) == 0) {
            handle_200();
        } else {
            handle_404();
        }

        puts("MESSAGE");
        puts(response);

        write(client_sock, response, strlen(response));
        puts("server message sent!");
        
        free(response);

        // 6. Close the socket after communicating
        close(client_sock);
}

/*
 - Create client connection 
   Reference: http://man7.org/linux/man-pages/man3/getaddrinfo.3.html#EXAMPLE
*/
int create_connection() {
    struct addrinfo hints, *result, *rp;
    int sock, s;
    int validfamily=0;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags    = AI_NUMERICSERV; /* numeric service number, not resolve */
    hints.ai_family   = AF_UNSPEC; /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Stream Socket */
    hints.ai_protocol = 0;          /* Any protocol */

    /* check for numeric IP to specify IPv6 or IPv4 socket */
    if ((validfamily = check_ipversion(remote_host))) {
         hints.ai_family = validfamily;
         hints.ai_flags |= AI_NUMERICHOST;  /* remote_host is a valid numeric ip, skip resolve */
    }

    /* Check if specified host is valid. Try to resolve address if remote_host is a hostname */
    if ((s = getaddrinfo(remote_host, remote_port , &hints, &result)) != 0) {
    
       /*
        The gai_strerror() function translates these error codes to a human
       readable string, suitable for error reporting.
       */
       fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
       exit(EXIT_FAILURE);
    }

    /*
    getaddrinfo() returns a list of address structures.
    Try each address until we successfully bind(2).
    If socket(2) (or bind(2)) fails, we (close the socket
    and) try the next address.
    */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
       sock = socket(rp->ai_family, rp->ai_socktype,
                    rp->ai_protocol);
       if (sock == -1)
           continue;

       if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1)
           break;                  /* Success */

       close(sock);
    }

    if (rp == NULL) {               /* No address succeeded */
       fprintf(stderr, "Could not connect\n");
       exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);           /* No longer needed */

    return sock;
}

/* Check for valid IPv4 or Iv6 string. Returns AF_INET family for IPv4, AF_INET6 family for IPv6 */
int check_ipversion(const char * address)
{
    /*
    NOTES:
        inet_pton() returns 1 on success (network address was successfully
        converted).  0 is returned if src does not contain a character string
        representing a valid network address in the specified address family.
        If af does not contain a valid address family, -1 is returned and
        errno is set to EAFNOSUPPORT.
    */

    // network address structure : compatible with both IPv4 and IPv6
    struct in6_addr bindaddr;

    if (inet_pton(AF_INET, address, &bindaddr) == 1) {
         return AF_INET;
    } else if (inet_pton(AF_INET6, address, &bindaddr) == 1) {
        return AF_INET6;
    } else 
        return 0;
}
