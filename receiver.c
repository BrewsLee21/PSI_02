#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <arpa/inet.h>

#include <unistd.h>

#include "network.h"

int main() {
    struct addrinfo *res, hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP
    hints.ai_flags = AI_PASSIVE; // Any available local address

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        fprintf(stderr, "ERROR: getaddrinfo() call failed!\n");
        return 1;
    }

    int my_socket;
    
    // Create socket and bind to the first available address
    struct addrinfo *p;
    for (p = res; p != NULL; p = p->ai_next) {
        if ((my_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }

        if (bind(my_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(my_socket);
            continue;
        }

        break;
    }
    
    freeaddrinfo(res);

    if (p == NULL) {
        fprintf(stderr, "ERROR: Failed to bind to an address!\n");
        return 1;
    }
    
    printf("Waiting...\n");

    if (recv_file(my_socket) == -1) {
        return 1;
    }

    printf("Done\n");
    

    close(my_socket);
    return 0;
}
