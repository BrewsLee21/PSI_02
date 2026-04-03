#include "common_net.h"

#ifndef RECEIVER_NET_H
#define RECEIVER_NET_H

#define INVALID_PACKET 102

int recv_file(int sock);
int recv_packet(peerinfo_t peer, packet_t *p);
int send_ack(peerinfo_t peer);
int send_nack(peerinfo_t peer);
int recv_init_packet(peerinfo_t *peer, packet_t *p);

void extract_start_data(packet_t *p, char *fname, uint32_t *fsize, unsigned char *hash);

#endif
