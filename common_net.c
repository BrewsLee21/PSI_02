#include <stdio.h>
#include <string.h>

#include "common_net.h"

char *TypeStr[] = {
    "ACK",
    "NACK",
    "START",
    "DATA",
    "END"
};

void serialize_packet(packet_t *p, char *buffer) {
    uint32_t type_net = htonl((uint32_t)p->type);
    uint32_t len_net = htonl(p->data_len);
    uint32_t crc_net = htonl(p->crc);
    uint32_t seq_net = htonl(p->seq);

    size_t offset = 0;
    memcpy(buffer + offset, &type_net, TYPE_SIZE);
    offset += TYPE_SIZE;
    memcpy(buffer + offset, &len_net, DATA_LEN_SIZE);
    offset += DATA_LEN_SIZE;
    memcpy(buffer + offset, p->hash, SHA256_DIGEST_LENGTH);
    offset += SHA256_DIGEST_LENGTH;
    memcpy(buffer + offset, &crc_net, CRC_SIZE);
    offset += CRC_SIZE;
    memcpy(buffer + offset, &seq_net, SEQ_SIZE);
    offset += SEQ_SIZE;
    memcpy(buffer + offset, p->data, p->data_len);
}

void deserialize_packet(char *buffer, packet_t *p) {
    uint32_t type_net, len_net, crc_net, seq_net;
    size_t offset = 0;
    memcpy(&type_net, buffer + offset, TYPE_SIZE);
    offset += TYPE_SIZE;
    memcpy(&len_net, buffer + offset, DATA_LEN_SIZE);
    offset += DATA_LEN_SIZE;
    memcpy(&p->hash, buffer + offset, SHA256_DIGEST_LENGTH);
    offset += SHA256_DIGEST_LENGTH;
    memcpy(&crc_net, buffer + offset, CRC_SIZE);
    offset += CRC_SIZE;
    memcpy(&seq_net, buffer + offset, SEQ_SIZE);
    offset += SEQ_SIZE;

    p->type = (enum PacketType)ntohl(type_net);
    p->data_len = ntohl(len_net);
    p->crc = ntohl(crc_net);
    p->seq = ntohl(seq_net);

    if (p->data_len > MAX_DATA_SIZE) {
        p->data_len = 0;
    }

    memcpy(p->data, buffer + offset, p->data_len);
}
