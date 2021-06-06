#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdint.h>

// UDP packets types
#define RESOURCE_LIST 0
#define DELETE_RESOURCE 1

// TCP commands
#define SEND_FILE 0
#define END_OF_FILE 1

struct DatagramHeader {
    uint32_t type;
};
struct DeleteDatagramHeader {
    struct DatagramHeader header;
    char resourceId[16];
};
struct ResourceDetails {
    struct DatagramHeader header;
    // char resourceId[16];
    uint32_t size;
    char name[48];
};

struct packetHeader{
    uint8_t command;
};
struct ResourcePacketHeader {
    uint8_t command;
    uint32_t offset;
    uint64_t size;
};

#endif