//
// Created by John Milimo on 2019-04-23.
//
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PORT 8088  //Where the clients can reach at

/*
 * Socket -> Bind -> listen -> accept -> read -> write -> read -> close
 */
int main(int argc, char const *argv[]) {

    int sockfd, new_socket, caddrlen;
    long valread;
    char *original_msg = "Kijana fupi nono round - Mulmulwas";

    char *message = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: %d\n\n";

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
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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
    address.sin_port = htons(PORT);

    if((bind(sockfd, (struct sockaddr *) &address, sizeof(address))) < 0) {
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

    if (listen(sockfd, 5) < 0) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    /*
     * 4. Accept connections
     *
     * int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
     *
     * The accept system call grabs the first connection request on the queue of pending connections (set up in listen) and
     * creates a new socket for that connection.
     * The original socket that was set up for listening is used only for accepting connections,
     * not for exchanging data. By default, socket operations are synchronous, or blocking, and accept will block
     * until a connection is present on the queue.
     */

    while(1){
        printf("-------------- Waiting for new connection ---------------\n");
        caddrlen = sizeof(caddress);
        if ((new_socket = accept(sockfd, (struct sockaddr *) &caddress, (socklen_t*)&caddrlen)) < 0) {
            perror("Error accepting");
            exit(EXIT_FAILURE);
        }

        // 5. Send and Receive messages
        char message_buffer[30000] = {0};
        valread = read(new_socket, message_buffer, 30000);
        printf("%s\n", message_buffer);

        write(new_socket, message, strlen(message));
        puts("server message sent!");

        // 6. Close the socket after communicating
        close(new_socket);

    }

}

