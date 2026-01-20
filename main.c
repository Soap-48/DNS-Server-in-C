#include <stdio.h>
#include <stdlib.h>

#include "packet.h"
#include "server.h"

int main() {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    Server server;
    if (server_init(&server, 2053) != 0) {
        return 1;
    }

    while (1) {
        unsigned char buffer[512];
        int bytesRead;
        // Receive data
        bytesRead = server_receive(&server, buffer, sizeof(buffer));
        if (bytesRead == -1) {
            perror("Error receiving data");
            break;
        }
        buffer[bytesRead] = '\0';

        print_hex(buffer, bytesRead);
        header *info = (header *)malloc(sizeof(header));
        parse_header(info, buffer, bytesRead);
        question *q = (question *)malloc(sizeof(question));
        q->domain = (char *)malloc(sizeof(char) * 256);
        parse_question(q, buffer);

        unsigned char out[128];
		answer *a= (answer*)malloc(sizeof(answer));
		int len=create_packet(info,q,a,out);
        print_hex(out, len);
        server_send(&server, out, len);
        
		//free all allocated data
        free(q->domain);
		free(a);
        free(q);
        free(info);
    }
    server_close(&server);
    return 0;
}