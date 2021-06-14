#ifndef BROADCAST_H
#define BROADCAST_H

#include <iostream>
#include <cstring>
#include <tuple>

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
    int sock; // deskryptor gniazda rozgloszeniowego
    unsigned short port; // port do rozglaszania
    struct sockaddr_in address; // address gniazda rozgloszeniowego
    std::string broadcastIp; // ip rozgloszeniowe
    std::string ip; // ip lokalne
public:
    Broadcast(const char *name, unsigned short port);
    ~Broadcast();
    void broadcast(struct ResourceDetails message); // wysylanie pakietow 
    ReceivedPacket receive(); // odbieranie pakietow
    void setIps(const char *name="eth0"); // znalezienie adresow ip w zaleznosci od wybranego interfejsu 
    std::string getIp(); // zwraca wartosc lokalnego ip
};

#endif
