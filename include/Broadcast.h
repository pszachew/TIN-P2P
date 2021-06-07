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
    unsigned short port;
    struct sockaddr_in address;
    std::string broadcastIp;
    std::string ip;
public:
    Broadcast(const char *name, unsigned short port);
    void broadcast(char const *message, int msgSize, int id);
    void broadcast(struct ResourceDetails message);
    struct ResourceDetails receive();
    void setIps(const char *name="eth0");
    std::string getIp();
};

#endif
