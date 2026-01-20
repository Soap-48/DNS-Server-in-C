#include "server.h"

#include <errno.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int server_init(Server *server, uint16_t port) {
    server->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server->socket_fd < 0) {
        perror("socket");
        return -1;
    }

    int reuse = 1;
    setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {htonl(INADDR_ANY)}
    };

    if (bind(server->socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server->socket_fd);
        return -1;
    }

    server->client_addr_len = sizeof(server->client_addr);
    return 0;
}

int server_receive(Server *server, void *buffer, size_t size) {
    return recvfrom(
        server->socket_fd,
        buffer,
        size,
        0,
        (struct sockaddr *)&server->client_addr,
        &server->client_addr_len);
}

int server_send(Server *server, const void *buffer, size_t size) {
    return sendto(
        server->socket_fd,
        buffer,
        size,
        0,
        (struct sockaddr *)&server->client_addr,
        server->client_addr_len);
}

void server_close(Server *server) {
    close(server->socket_fd);
}
