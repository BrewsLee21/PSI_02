#include <stdio.h>
#include <string.h>

#include <errno.h>

#include "utils.h"
#include "sender_net.h"

#define VERBOSE 1

int send_file(peerinfo_t peer, char *fpath) {
    FILE *f = fopen(fpath, "rb");
    if (f == NULL) {
        fprintf(stderr, "ERROR: get_file_hash: file %s couldn't be opened!\n", fpath);
        return ERROR;
    }
    
    int bytes_read;

    // Send START packet
    if (send_init_packet(peer, fpath, f) == -1) {
        fprintf(stderr, "ERROR: send_file: File transfer failed!\n");
        return ERROR;
    }

    packet_t p;
    char buffer[MAX_PACKET_BUFFER_SIZE];
    char chunk[MAX_DATA_SIZE];
    
    // Read entire file one chunk at a time
    while (1) {
        bytes_read = fread(chunk, sizeof(char), MAX_DATA_SIZE, f);
    
        if (bytes_read == 0) {
            // End of file
            if (feof(f) != 0) {
                break;
            } else {
                fprintf(stderr, "ERROR: send_file: Reading file failed!\n");
                return ERROR;
                
            }
        }

        // Send DATA packet
        p.type = DATA;
        p.data_len = bytes_read;
        memcpy(p.data, chunk, bytes_read);
        p.crc = get_crc(p.data, p.data_len);
    
        if (send_packet(peer, &p, buffer) == -1) {
            fprintf(stderr, "ERROR: send_file: File transfer failed!\n");
            return ERROR;
        }
    }

    // Send END packet
    p.type = END;
    p.data_len = 0;
    p.data[0] = 0x00;
    p.crc = get_crc(p.data, p.data_len);

    if (send_packet(peer, &p, buffer) == ERROR) {
        fprintf(stderr, "ERROR: send_file: File transfer failed!\n");
        return ERROR;
    }

    fclose(f);
    return 0;
}

int send_packet(peerinfo_t peer, packet_t *p, char *buffer) {
    serialize_packet(p, buffer);
    while (1) {
        int bytes_sent = send(peer.sock, buffer, HEADER_SIZE + p->data_len, 0);
        if (bytes_sent == -1) {
            return ERROR;
        }

        if (VERBOSE) {
            printf("INFO: packet type %s sent!\n", TypeStr[p->type]);
        }
    
        int status = recv_ack(peer);
        if (status == ACK_NOT_RECEIVED) { // ACK not received
            continue; // resend packet again
        } else if (status == ERROR) {
            fprintf(stderr, "ERROR: send_packet: File transfer failed!\n");
            return ERROR;
        }
        
        break;
    }

    return 0;
}

int recv_ack(peerinfo_t peer) {
    char buffer[MAX_PACKET_BUFFER_SIZE];

    packet_t p;

    struct sockaddr_in new_addr;
    socklen_t new_addr_len = sizeof(new_addr);

    
    if (recvfrom(peer.sock, buffer, MAX_PACKET_BUFFER_SIZE, 0, (struct sockaddr *)&new_addr, &new_addr_len) == -1) {
        // ACK timeout
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            if (VERBOSE) {
                printf("\tINFO: ACK not received! Resending packet...\n");
            }
            return ACK_NOT_RECEIVED;
        }
        return ERROR;
    }

    deserialize_packet(buffer, &p);

    // Check if address matches
    if (peer.addr_len != new_addr_len ||
        ipcmp(&peer.addr, &new_addr) != 0
    ) {
        if (VERBOSE) {
            printf("\tINFO: ACK received from different address! Resending packet...\n");
        }
        return ACK_NOT_RECEIVED;
    }

    if (p.type == NACK) {
        if (VERBOSE) {
            printf("\tINFO: NACK received! Resending packet...\n");
        }
        return ACK_NOT_RECEIVED;
    } else if (p.type != ACK) {
        if (VERBOSE) {
            printf("\tINFO: Invalid packet type received! Resending packet...\n");
        }
        return ACK_NOT_RECEIVED;
    }
    
    if (VERBOSE) {
        printf("\tINFO: ACK received!\n");  
    }
    return 0;
}

int send_init_packet(peerinfo_t peer, char *fpath, FILE *stream) {
    char *fname = get_filename(fpath);
    uint32_t fsize = get_file_size(stream);
    int fsize_len = snprintf(NULL, 0, "%u", fsize);
    
    packet_t p;
    p.type = START;
    p.data_len = fsize_len + 1 + strlen(fname) + 1;
    snprintf(p.data, p.data_len, "%s\n%u", fname, fsize);

    get_file_hash(stream, p.hash);
    p.crc = get_crc(p.data, p.data_len);

    if (VERBOSE) {
        printf("File hash: ");
        print_hex_hash(p.hash);
    }
    
    // TODO: crc

    char buffer[MAX_PACKET_BUFFER_SIZE];
    
    if (send_packet(peer, &p, buffer) == ERROR) {
        fprintf(stderr, "ERROR: send_init_packet: File transfer failed!\n");
        return ERROR;
    }

    return 0;
}
