#include <stdio.h>
#include <string.h>

#include "common_net.h"

void serialize_packet(packet_t *p, char *buffer) {
    uint32_t type_net = htonl((uint32_t)p->type);
    uint32_t len_net = htonl(p->data_len);
    uint32_t crc_net = htonl(p->crc);

    size_t offset = 0;
    memcpy(buffer + offset, &type_net, TYPE_SIZE);
    offset += TYPE_SIZE;
    memcpy(buffer + offset, &len_net, DATA_LEN_SIZE);
    offset += DATA_LEN_SIZE;
    memcpy(buffer + offset, p->hash, SHA256_DIGEST_LENGTH);
    offset += SHA256_DIGEST_LENGTH;
    memcpy(buffer + offset, &crc_net, CRC_SIZE);
    offset += CRC_SIZE;
    memcpy(buffer + offset, p->data, p->data_len);
}

void deserialize_packet(char *buffer, packet_t *p) {
    uint32_t type_net, len_net, crc_net;
    size_t offset = 0;
    memcpy(&type_net, buffer + offset, TYPE_SIZE);
    offset += TYPE_SIZE;
    memcpy(&len_net, buffer + offset, DATA_LEN_SIZE);
    offset += DATA_LEN_SIZE;
    memcpy(&p->hash, buffer + offset, SHA256_DIGEST_LENGTH);
    offset += SHA256_DIGEST_LENGTH;
    memcpy(&crc_net, buffer + offset, CRC_SIZE);
    offset += CRC_SIZE;
    
    
    p->type = (enum PacketType)ntohl(type_net);
    p->data_len = ntohl(len_net);
    p->crc = ntohl(crc_net);
    memcpy(p->data, buffer + offset, p->data_len);
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
