#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include <zlib.h>
#include <openssl/sha.h>

#include "utils.h"

char *get_filename(char* path) {
    char *filename = strrchr(path, '/'); // Get pointer to last occurence of '/'
    return filename ? filename + 1 : path; // Return pointer + 1 (filename withuot '/' char) or the entire path if there is no '/' char
}

uint32_t get_file_size(FILE *stream) {
    fseek(stream, 0, SEEK_END); // Go to end of stream
    long size = ftell(stream); // Get position (equivalent to file size)
    rewind(stream); // Go back to the beginning of stream

    return (uint32_t)size;
}

int get_file_hash(FILE *stream, unsigned char *hash) {
    rewind(stream);

    uint32_t fsize = get_file_size(stream);

    unsigned char data[fsize];

    if (fread(data, sizeof(unsigned char), fsize, stream) == 0) {
        if (ferror(stream)) {
            fprintf(stderr, "ERROR: get_file_hash: Reading file failed!\n");
            return -1;
        }
    }

    SHA256(data, fsize, hash);

    rewind(stream);
    return 0;
}

int hashcmp(unsigned char *hash1, unsigned char *hash2) {
    return memcmp(hash1, hash2, SHA256_DIGEST_LENGTH);
}

void print_hex_hash(unsigned char *hash) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}
