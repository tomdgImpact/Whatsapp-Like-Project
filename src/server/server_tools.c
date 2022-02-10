#define _GNU_SOURCE

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <err.h>

#include "server_tools.h"

// establish a connection between
// the server and a client
void *linkit(void *data)
{
    SOCKET_FD *fds_heap;
    SOCKET_FD fds_stack;
    char chatbuff[BUFFER_SIZE];
    char clibuff[BUFFER_SIZE];

    memset(&fds_stack, 0, sizeof(SOCKET_FD));
    fds_heap = data;
    fds_stack.host = fds_heap->host;
    fds_stack.dist = fds_heap->dist;

    // zero the buffers
    memset(chatbuff, 0, BUFFER_SIZE);
    memset(clibuff, 0, BUFFER_SIZE);

    snprintf(clibuff, BUFFER_SIZE, "CLIENT >> CONNECTED AS [%i]\n",
                fds_stack.host);

    // print the connection status
    write(fds_stack.host, &clibuff, BUFFER_SIZE);

    int r;
    while ((r = read(fds_stack.host, &chatbuff, BUFFER_SIZE)) != 0)
    {
        // check for error
        if (r == -1) write(fds_stack.host, "CLIENT >> TRY AGAIN\n", 20);

        // check for exit command
        if (strcmp(chatbuff, "EXIT\n") == 0) break;

        // send the message
        int w;
        //write(fds_stack.dist, "\nFRIEND >> ", 11);
        w = write(fds_stack.dist, chatbuff, r);
        if (w == -1)
            write(fds_stack.host, "CLIENT >> FRIEND LEFT\n", 22);

        memset(chatbuff, 0, BUFFER_SIZE);
    }

    // announce that friend left
    write(fds_stack.dist, "CLIENT >> FRIEND LEFT\n", 22);
    close(fds_stack.host);

    return NULL;
}

// initialize the file descriptor
// to listen for connections
FD init_serv(void)
{
    FD socketfd;
    struct addrinfo *addr;
    struct addrinfo *copy;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));

    // set the parameters for connections
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // handling any error
    if (getaddrinfo(NULL, PORT_NUMBER, &hints, &addr) != 0)
        errx(EXIT_FAILURE, "SERVER >> ERROR ON SERVER INITIALIZATION\n");

    for (copy = addr; copy != NULL; copy = copy->ai_next)
    {
        socketfd = socket(copy->ai_family,
            copy->ai_socktype, copy->ai_protocol);

        if (socketfd == -1) continue;

        int enable = 1;
        int err = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, 
                    &enable, sizeof(int));

        if (err == -1) errx(EXIT_FAILURE, "SERVER >> ERROR IN SOCKET OPTIONS");
        if (bind(socketfd, copy->ai_addr, copy->ai_addrlen) != -1) break;

        close(socketfd);
    }

    if (copy == NULL)
        errx(EXIT_FAILURE, "SERVER >> ERROR ON SERVER CREATION\n");

    freeaddrinfo(addr);

    return socketfd;
}
