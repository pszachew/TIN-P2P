#ifndef BROADCAST_H
#define BROADCAST_H

#include <iostream>
#include <cstring>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ifaddrs.h>

#include "Structures.h"


class Broadcast {
private:
    int sock;
    const char *ip;
    unsigned short port;
    struct sockaddr_in address;
public:
    Broadcast(char const* broadcastIp, unsigned short port);
    void broadcast(char const *message, int msgSize, int id);
    void broadcast(struct ResourceDetails message);
    struct ResourceDetails receive();
};

#endif
