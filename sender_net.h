#include <stdio.h>

#include "common_net.h"

#ifndef SENDER_NET_H
#define SENDER_NET_H

#define ACK_TIMEOUT 5
#define ACK_NOT_RECEIVED 101

int send_file(peerinfo_t peer, char *fpath);
int send_packet(peerinfo_t peer, packet_t *p, char *buffer);
int recv_ack(peerinfo_t peer);
int send_init_packet(peerinfo_t peer, char *fpath, FILE *stream);

#endif
