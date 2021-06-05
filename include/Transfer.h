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
public:
    Transfer();
    void transfer();
    void receive();
};

#endif