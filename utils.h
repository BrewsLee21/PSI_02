#include <stdio.h>
#include <stdint.h>

#ifndef MY_UTILS_H
#define MY_UTILS_H

int get_file_hash(FILE *stream, unsigned char *hash);
char *get_filename(char* path);
uint32_t get_file_size(FILE *stream);
int hashcmp(unsigned char *hash1, unsigned char *hash2);
void print_hex_hash(unsigned char *hash);

#endif
