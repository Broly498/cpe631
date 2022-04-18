#pragma once

// C/C++ Includes
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdexcept>

// Linux Includes
#include <sys/mman.h>

namespace cpe631
{
static inline uint64_t get_address(char* identifier)
{
    FILE * fp;
    char * line = NULL;
    ssize_t read;
    size_t len;
    char *tmp;
    uint64_t address = 0x0;
    uint8_t identifier_len = strlen(identifier);

    fp = fopen("/proc/kallsyms", "r");
    if (fp == NULL){
        return address;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        tmp = strstr(line, identifier);
        if (tmp != NULL && (strlen(tmp) == (identifier_len + 1)))
        {
            address = strtoull(strtok(line, " "), NULL, 16);

            break;
        }
        // if(strlen(line) > identifier_len && !strcmp(line + strlen(line) - identifier_len, identifier)){
        //         address = strtoull(strtok(line, " "), NULL, 16);
        //         break;
        // }
    }

    fclose(fp);
    if (line){
        free(line);
    }
    return address;
}
}