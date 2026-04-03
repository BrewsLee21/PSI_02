#include <stdio.h>
#include <stdint.h>

#ifndef MY_UTILS_H
#define MY_UTILS_H

char *get_filename(char* path);
uint32_t get_file_size(FILE *stream);

// Uloha 2
int get_file_hash(FILE *stream, unsigned char *hash);
int hashcmp(unsigned char *hash1, unsigned char *hash2);
void print_hex_hash(unsigned char *hash);

uint32_t get_crc(const char *data, size_t data_len);
int check_crc(uint32_t received_crc, const char *data, size_t data_len);

#endif
