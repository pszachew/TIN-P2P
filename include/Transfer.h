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


class Transfer {
private:
    int sock;
    struct sockaddr_in address;

public:
    Transfer(std::string filename, int port, std::string ip);
    void send();
    void receive();
};

#endif