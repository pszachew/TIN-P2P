#ifndef TRANSFER_H
#define TRANSFER_H


#include <iostream>
#include <cstring>
#include <fstream>

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
    int port;
    struct sockaddr_in address;
    const char* filename;
    std::ofstream *logFile;

public:
    Transfer(const char* filename, std::ofstream *logFile, std::string ip, bool sending, int port=0);
    int getPort();
    void sendFile();
    void receive();
};

#endif
