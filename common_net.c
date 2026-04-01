#include <stdio.h>
#include <string.h>

#include "common_net.h"

void serialize_packet(packet_t *p, char *buffer) {
    uint32_t type_net = htonl((uint32_t)p->type);
    uint32_t len_net = htonl(p->data_len);

    memcpy(buffer, &type_net, sizeof(type_net));
    memcpy(buffer + 4, &len_net, sizeof(len_net));
    memcpy(buffer + 8, p->data, p->data_len);
}

void deserialize_packet(char *buffer, packet_t *p) {
    uint32_t type_net, len_net;
    memcpy(&type_net, buffer, 4);
    memcpy(&len_net, buffer + 4, 4);
    
    p->type = (enum PacketType)ntohl(type_net);
    p->data_len = ntohl(len_net);
    memcpy(p->data, buffer + 8, p->data_len);
}

int ipcmp(struct sockaddr_in *ip1, struct sockaddr_in *ip2) {
    if (ip1->sin_family == ip2->sin_family &&
        ip1->sin_port == ip2->sin_port &&
        ip1->sin_addr.s_addr == ip2->sin_addr.s_addr
    ) {
        return 0;
    }

    return 1;
}
