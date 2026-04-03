
#include <stdio.h>
#include <string.h>

#include <errno.h>

#include "utils.h"
#include "receiver_net.h"

#define VERBOSE 1

int recv_file(int sock) {
    peerinfo_t peer = {0};
    peer.sock = sock;
    peer.addr_len = sizeof(peer.addr);

    // Receive START packet
    packet_t p;
    int status;
    
    do {
        status = recv_init_packet(&peer, &p);
        if (status == ERROR) {
            fprintf(stderr, "ERROR: recv_file: File reception failed!\n");
            return ERROR;
        }
    } while (status != 0);


    char fname[MAX_FPATH_SIZE + 1];
    uint32_t fsize;
    unsigned char hash[SHA256_DIGEST_LENGTH]; 

    extract_start_data(&p, fname, &fsize, hash); // get file name, size and hash

    FILE *f = fopen(fname, "wb");

    if (f == NULL) {
        fprintf(stderr, "ERROR: recv_file: file %s couldn't be opened!\n", fname);
        return ERROR;
    }

    int bytes_written;
    
    // Receive DATA packets
    while (1) {
        status = recv_packet(peer, &p);
        if (status == ERROR) {
            fprintf(stderr, "ERROR: recv_file: File reception failed!\n");
            return ERROR;
        }

        if (status == INVALID_PACKET) {
            if (VERBOSE) {
                printf("INFO: Received packet was invalid!\n");
            }
            continue;
        }

        if (p.type == END) {
            break;
        } else if (p.type == DATA) {
            bytes_written = fwrite(p.data, sizeof(char), p.data_len, f);
            if (bytes_written == 0) {
                fprintf(stderr, "ERROR: recv_file: Writing file failed!\n");
                fclose(f);
            }
        }
        // else { invalid packet }
    }

    unsigned char hash_check[SHA256_DIGEST_LENGTH];

    fclose(f);

    f = fopen(fname, "rb");

    get_file_hash(f, hash_check);

    fclose(f);

    if (VERBOSE) {
        printf("File hash: ");
        print_hex_hash(hash_check);
    }
    
    // Compare initial hash with my calculated hash
    if (hashcmp(hash, hash_check) != 0) {
        printf("WARNING: recv_file: Generated hash and received hash do not match!\n");
        return ERROR;
        // TODO: Ask to receive the file again or something...
        // received file corrupted
    }

    if (VERBOSE) {
        printf("INFO: hashcmp passed!\n");
    }

    return 0;
}

int recv_packet(peerinfo_t peer, packet_t *p) {
    char buffer[MAX_PACKET_BUFFER_SIZE];

    struct sockaddr_in new_addr;
    socklen_t new_addr_len = sizeof(new_addr);
    
    if (recvfrom(peer.sock, buffer, MAX_PACKET_BUFFER_SIZE, 0, (struct sockaddr *)&new_addr, &new_addr_len) == -1) {
        fprintf(stderr, "ERROR: recv_packet: Packet reception failed!\n");
        return ERROR;
    }

    // Check peer address
    if (peer.addr_len != new_addr_len ||
        ipcmp(&peer.addr, &new_addr) != 0
    ) {
        return INVALID_PACKET;
    }

    deserialize_packet(buffer, p);

    // CRC doesn't match => data is corrupted
    if (check_crc(p->crc, p->data, p->data_len) == 0) {
        if (VERBOSE) {
            printf("\tINFO: CRC mismatch!\n");
        }
        if (send_nack(peer) == ERROR) {
            fprintf(stderr, "ERROR: recv_init_packet: Failed to send NACK!\n");
            return ERROR;
        }
        return INVALID_PACKET;
    }

    if (VERBOSE) {
        if (p->type == END) {
            printf("INFO: END packet!\n");
        } else if (p->type == DATA) {
            printf("INFO: DATA packet: %u\n", p->data_len);
        }
    }

    if (send_ack(peer) == ERROR) {
        fprintf(stderr, "ERROR: recv_packet: Failed to send ACK!\n");
        return ERROR;
    }
    
    return 0;
}

int send_ack(peerinfo_t peer) {
    packet_t ack;
    ack.type = ACK;
    ack.data_len = 0;
    ack.data[0] = 0x00;

    char buffer[MAX_PACKET_BUFFER_SIZE];

    serialize_packet(&ack, buffer);

    if (sendto(peer.sock, buffer, HEADER_SIZE + ack.data_len, 0, (struct sockaddr *)&peer.addr, peer.addr_len) == -1) {
        fprintf(stderr, "ERROR: send_ack: Failed to send ACK!\n");
        return ERROR;
    }

    if (VERBOSE) {
        printf("INFO: ACK sent!\n");
    }

    return 0;
}

int send_nack(peerinfo_t peer) {
    packet_t nack;
    nack.type = NACK;
    nack.data_len = 0;
    nack.data[0] = 0x00;

    char buffer[MAX_PACKET_BUFFER_SIZE];

    serialize_packet(&nack, buffer);

    if (sendto(peer.sock, buffer, HEADER_SIZE + nack.data_len, 0, (struct sockaddr *)&peer.addr, peer.addr_len) == -1) {
        fprintf(stderr, "ERROR: Failed to send NACK!\n");
        return ERROR;
    }

    if (VERBOSE) {
        printf("INFO: NACK sent!\n");
    }

    return 0;
}

int recv_init_packet(peerinfo_t *peer, packet_t *p) {
    char buffer[MAX_PACKET_BUFFER_SIZE];
        
    int msg_len = recvfrom(peer->sock, buffer, MAX_PACKET_BUFFER_SIZE, 0, (struct sockaddr *)&peer->addr, &peer->addr_len);

    if (msg_len == -1) {
        fprintf(stderr, "ERROR: recv_init_packet: START packet not received!\n");
        return ERROR;
    }

    if (VERBOSE) {
        printf("INFO: START packet received!\n");
    }

    deserialize_packet(buffer, p);

    // CRC doesn't match => data is corrupted
    if (check_crc(p->crc, p->data, p->data_len) == 0) {
        if (send_nack(*peer) == ERROR) {
            fprintf(stderr, "ERROR: recv_init_packet: Failed to send NACK!\n");
            return ERROR;
        }
        return INVALID_PACKET;
    }

    if (send_ack(*peer) == ERROR) {
        fprintf(stderr, "ERROR: recv_init_packet: Failed to send ACK!\n");
        return ERROR;
    }
    return 0;
}

void extract_start_data(packet_t *p, char *fname, uint32_t *fsize, unsigned char *hash) {
    sscanf(p->data, "%" XSTR(MAX_FPATH_SIZE) "s %u", fname, fsize);
    memcpy(hash, p->hash, SHA256_DIGEST_LENGTH); // get file hash    
}
