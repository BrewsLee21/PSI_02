#include <stdio.h>

#include "common_net.h"

#ifndef SENDER_NET_H
#define SENDER_NET_H

#define ACK_TIMEOUT 2
#define ACK_NOT_RECEIVED 101

#define SENDER_LOCAL_PORT "5333"   // Port the sender binds to locally
#define SENDER_TARGET_PORT "5111"  // Port the sender sends to (Netderper or Receiver)

int send_file(peerinfo_t peer, char *fpath);
int send_packet(peerinfo_t peer, packet_t *p, char *buffer);
int recv_ack(peerinfo_t peer);
int send_init_packet(peerinfo_t peer, char *fpath, FILE *stream);

#endif
