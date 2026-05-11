#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <unistd.h>

#include "receiver_net.h"

int main() {
    // Create socket
    int my_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (my_socket == -1) {
        perror("socket");
        return 1;
    }

    // Allow reuse of local port (avoids TIME_WAIT issues on restart)
    int opt = 1;
    if (setsockopt(my_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt SO_REUSEADDR");
        close(my_socket);
        return 1;
    }

    // Bind to local port
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(atoi(RECEIVER_LOCAL_PORT));
    local_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(my_socket, (struct sockaddr *)&local_addr, sizeof(local_addr)) == -1) {
        perror("bind");
        close(my_socket);
        return 1;
    }

    printf("Waiting...\n");

    if (recv_file(my_socket) == -1) {
        close(my_socket);
        return 1;
    }

    printf("Done\n");

    close(my_socket);
    return 0;
}
