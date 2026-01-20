#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdint.h>

typedef struct {
    int socket_fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
} Server;

int server_init(Server *server, uint16_t port);
void server_close(Server *server);
int server_receive(Server *server, void *buffer, size_t size);
int server_send(Server *server, const void *buffer, size_t size);

#endif
