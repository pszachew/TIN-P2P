#ifndef TRANSFER_H
#define TRANSFER_H


#include <iostream>
#include <cstring>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "Structures.h"

class Transfer {
private:
    int sock;
    struct sockaddr_in address;
    const char* filename;
    void sendFile();
    void receive();

public:
    Transfer(const char* filename, int port, std::string ip, bool sending);
};

#endif
