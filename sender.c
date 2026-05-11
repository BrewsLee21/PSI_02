#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <unistd.h>

#include "sender_net.h"

int main() {
    char receiver_addr[INET_ADDRSTRLEN];

    printf("Enter receiver IP address: ");
    scanf("%s", receiver_addr);
    getchar(); // Consume '\n'

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
    local_addr.sin_port = htons(atoi(SENDER_LOCAL_PORT));
    local_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(my_socket, (struct sockaddr *)&local_addr, sizeof(local_addr)) == -1) {
        perror("bind");
        close(my_socket);
        return 1;
    }

    // Set ACK receive timeout
    struct timeval tv;
    tv.tv_sec = ACK_TIMEOUT;
    tv.tv_usec = 0;
    if (setsockopt(my_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
        perror("setsockopt SO_RCVTIMEO");
        close(my_socket);
        return 1;
    }

    // Set up target (receiver/Netderper) address
    struct sockaddr_in target_addr;
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(atoi(SENDER_TARGET_PORT));
    if (inet_pton(AF_INET, receiver_addr, &target_addr.sin_addr) != 1) {
        fprintf(stderr, "ERROR: Invalid IP address: %s\n", receiver_addr);
        close(my_socket);
        return 1;
    }

    printf("Sending to: %s\n", receiver_addr);

    peerinfo_t peer;
    peer.sock = my_socket;
    peer.addr = target_addr;
    peer.addr_len = sizeof(target_addr);

    char fpath[MAX_FPATH_SIZE + 1];
    printf("Enter file path (%d bytes): ", MAX_FPATH_SIZE);
    scanf("%" XSTR(MAX_FPATH_SIZE) "s", fpath);

    printf("Sending file: %s\n", fpath);

    if (send_file(peer, fpath) == -1) {
        close(my_socket);
        return 1;
    }

    printf("File transmitted successfully!\n");

    close(my_socket);
    return 0;
}
