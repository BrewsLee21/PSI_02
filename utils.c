#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

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



