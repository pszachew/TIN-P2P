#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdint.h>

// UDP packets types
#define RESOURCE_LIST 0
#define DELETE_RESOURCE 1
#define DOWNLOAD_REQUEST 2

// TCP commands
#define SEND_FILE 0
#define END_OF_FILE 1

#define CHUNK_SIZE 1024

struct DeleteDatagramHeader {
    uint32_t type;
    char name[48];
};
struct ResourceDetails {
    uint32_t type;
    uint32_t size;
    char name[48];
};

struct packetHeader{
    uint8_t command;
};
struct ResourcePacketHeader {
    uint8_t command;
    // uint32_t offset;
    uint64_t size;
    char data[CHUNK_SIZE];
};

#endif
