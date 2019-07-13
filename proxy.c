/* Forward data between sockets */
/*Reference:  https://github.com/kklis/proxy/blob/master/proxy.c */

#include "proxy.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 16384
#define BROKEN_PIPE_ERROR -9

int forward_data(int source_sock, int destination_sock) {

    ssize_t msg_length_in_bytes;
    char buffer[BUF_SIZE];

    /*

    Upon successful completion, recv() shall return the length of the message
     in bytes. If no messages are available to be received and the peer 
     has performed an orderly shutdown, recv() shall return 0. 
     Otherwise, -1 shall be returned and errno set to indicate the error.

    */
     while ((msg_length_in_bytes = recv(source_sock, buffer, BUF_SIZE, 0)) > 0) { // read data from input socket
        send(destination_sock, buffer, msg_length_in_bytes, 0); // send data to output socket
    }

    if (msg_length_in_bytes < 0) {
        puts("ERROR: could not receive from source_sock");
        exit(BROKEN_PIPE_ERROR);
    }

    /* Ref:  https://linux.die.net/man/2/shutdown */
    shutdown(destination_sock, SHUT_RDWR); // stop other processes from using socket
    close(destination_sock);

    shutdown(source_sock, SHUT_RDWR); // stop other processes from using socket
    close(source_sock);

    return 1;
}
