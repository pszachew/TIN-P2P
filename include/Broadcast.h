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
#include <pthread.h>


class Broadcast {
private:
    int sock;
    char *ip;
    unsigned short port;
    struct sockaddr_in address;
    std::string findBroadcastIp(char const *name);
public:
    Broadcast(unsigned short port, char const *interface);
    void broadcast(char const *message, int msgSize, int id);
    std::string receive();
};

#endif