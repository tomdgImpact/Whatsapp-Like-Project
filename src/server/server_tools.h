#ifndef _SERVER_TOOLS_H_
#define _SERVER_TOOLS_H_

#define BUFFER_SIZE 128
#define BACKLOG 5
#define PORT_NUMBER "25000"

typedef int FD;

typedef struct {
    FD host; // host to server
    FD dist; // dist to server
} SOCKET_FD;

void *linkit(void *);

FD init_serv(void);

#endif /* !_SERVER_TOOLS_H_ */