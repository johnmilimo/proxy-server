//
// Created by John Milimo on 2019-04-23.
//

#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8088

/*
 *
 * Client need not bind, listen, and accept
 * All client need to do is to connect
 *
 * int connect(int sockfd, const struct sockaddr saddr, socklen_t addrlen);
 */

int main(int argc, char const *argv[]) {

    int sockfd;
    long valread;
    struct sockaddr_in serv_addr;
    char *message = "Hey Lonyangapuo eti unasema kijana gani?";

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creating socket");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);



    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    char buffer[30000] = {0};
    send(sockfd , message , strlen(message) , 0 );
    puts("client message sent!");
    valread = read( sockfd , buffer, 30000);
    printf("%s\n",buffer );

}