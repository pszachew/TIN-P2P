#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>
#include <set>
#include <stdint.h>

// UDP packets types
#define RESOURCE_LIST 0
#define DELETE_RESOURCE 1
#define DOWNLOAD_REQUEST 2
#define SELF_SEND 3
#define TIME_OUT 4

// TCP commands
#define SEND_FILE 0
#define END_OF_FILE 1

#define CHUNK_SIZE 1024

struct ResourceDetails {
    uint32_t type;
    uint32_t port;
    char name[48];
};

struct ReceivedPacket {
    ResourceDetails packet;
    std::string ip;
    ReceivedPacket() {}
    ReceivedPacket(ResourceDetails p, std::string i) : packet(p), ip(i){}
};

#endif
