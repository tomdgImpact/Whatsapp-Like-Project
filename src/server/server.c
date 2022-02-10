#define _GNU_SOURCE

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <err.h>
#include <time.h>
#include <errno.h>

#include "server_tools.h"
#include "../encryption/diffie-hellman.h"

void key_exchange(int fd1, int fd2)
{
    ull P = 3211891519;
    ull G = rand();
    ull buf[2] = {P, G};
    if (write(fd1, buf, 2) == -1 || write(fd2, buf, 2) == -1)
        errx(EXIT_FAILURE, "cannot send buffer for key_exchange");
    sleep(1);
    ull res[1];
    ull res2[1];
    if (read(fd1, res, 1) == -1 || read(fd2, res2, 1) == -1)
        errx(EXIT_FAILURE, "cannot read data from client for key_exchange");
    if (write(fd1, res2, 2) == -1 || write(fd2, res, 2) == -1)
        errx(EXIT_FAILURE, "cannot send buffer 2 for key_exchange");
    read(fd1, res, 1);
    if (res[0] == 1)
        key_exchange(fd1, fd2);
}

int main(void)
{
    FD socketfd = init_serv();

    pthread_t thrd_c_one;
    pthread_t thrd_c_two;

    SOCKET_FD client1;
    SOCKET_FD client2;

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    listen(socketfd, BACKLOG);

    if (fork() == 0) // child process
    {
        while (1)
        {
            memset(&client1, 0, sizeof(SOCKET_FD));
            memset(&client2, 0, sizeof(SOCKET_FD));

            // wait for client_1
            int sfd1 = accept(socketfd, NULL, NULL);
            printf("SERVER >> CLIENT [%i] CONNECTED\n", sfd1);

            // Queue message for client_1
            //write(sfd1, "SERVER >> YOU'RE ENQUEUED...\n", 30);

            // wait for client_2
            int sfd2 = accept(socketfd, NULL, NULL);
            printf("SERVER >> CLIENT [%i] CONNECTED\n", sfd2);
            // check for errors
            if (sfd1 == -1 || sfd2 == -1)
                printf("SERVER >> ON CLIENT CONNECTION ERROR\n");

            // connect the clients
            client1.host = sfd1;
            client1.dist = sfd2;

            client2.host = sfd2;
            client2.dist = sfd1;
            //key_exchange(sfd1, sfd2);
            int e1 = pthread_create(&thrd_c_one, NULL, (void *)linkit, &client1);
            int e2 = pthread_create(&thrd_c_two, NULL, (void *)linkit, &client2);

            // check the thread errors
            if (e1 != 0 || e2 != 0)
                errx(EXIT_FAILURE, "SERVER >> MULTITHREADING FAILURE\n");

            // make thread give memory back on finish
            pthread_detach(thrd_c_one);
            pthread_detach(thrd_c_two);

            sleep(1);
        }
    }
    else // parent process
    {
        printf("SERVER >> WAITING FOR CONNECTIONS OR COMMANDS...\n");
        int r;
        while ((r = read(STDIN_FILENO, buffer, BUFFER_SIZE)))
        {
            if (r == -1)
                errx(EXIT_FAILURE, "SERVER >> READ ERROR");

            if (r == BUFFER_SIZE)
            {
                memset(buffer, 0, BUFFER_SIZE);
                printf("SERVER >> TOO LONG COMMAND\n");
            }
            else
            {
                printf("SERVER >> WAITING FOR COMMAND\n");
                if (strncmp("SHUTDOWN", buffer, 8) == 0)
                {
                    // kill each process of the process group
                    // of the parent of the processes with SIGKILL
                    printf("SERVER >> STOPPED\n");
                    kill(0, 9);
                }
            }
        }
    }

    close(socketfd);
    return EXIT_SUCCESS;
}
