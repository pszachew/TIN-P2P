#ifndef TRANSFER_H
#define TRANSFER_H


#include <iostream>
#include <cstring>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "Structures.h"

class Transfer {
private:
    int sock; // deskryptor gniazda
    int port; // port polaczenia TCP
    struct sockaddr_in address; // adres gniazda
    std::string filename; // nazwa przesylanego pliku
    std::ofstream *logFile;

public:
    Transfer(std::string filename, std::ofstream *logFile, std::string ip, bool sending, int port=0);
    int getPort();
    void sendFile(); // wysylanie pliku do klienta
    void receive(); // odbieranie pliku od klienta
};

#endif
