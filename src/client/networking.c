#define _GNU_SOURCE

#include <arpa/inet.h>
#include <err.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

#include "networking.h"






#define BUFFER_SIZE 128
#define PORT_NUMBER "25000"
#define SERVER_IP   "176.158.234.48"






int establish_connection()
{
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *cpy;
    int file_desc = 0;
    int error_status;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    error_status = getaddrinfo(SERVER_IP, PORT_NUMBER, &hints, &res);

    if (error_status != 0)
    {
        errx(EXIT_FAILURE, "Error on finding socket addresses : %s",
             gai_strerror(error_status));
    }

    for (cpy = res; cpy != NULL; cpy = cpy->ai_next)
    {
        file_desc = socket(cpy->ai_family, cpy->ai_socktype, cpy->ai_protocol);

        if (file_desc == -1)
        {
            continue;
        }

        if (connect(file_desc, cpy->ai_addr, cpy->ai_addrlen) != -1)
        {
            break;
        }

        close(file_desc);
    }

    freeaddrinfo(res);


    return file_desc;
}
