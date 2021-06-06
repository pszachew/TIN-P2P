#include "Transfer.h"


Transfer::Transfer(std::string filename, int port, std::string ip="127.0.0.1"){
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("socket() failed");
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip.c_str());
}

void Transfer::send(){
    if(connect(sock, (struct sockaddr*)&address, sizeof(address)) < 0){
        perror("Unable to connect");
    }
    printf("Connected with server successfully\n");
}

void Transfer::receive(){
    if (bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0)
        perror("bind() failed");

    if(listen(sock, 1) < 0)
        perror("Error while listening");

    struct sockaddr_in clientAddress;
    socklen_t len = sizeof(clientAddress);
    int receivedSock = accept(sock, (struct sockaddr*) &clientAddress, &len);
    if (receivedSock < 0)
        perror("Can't accept\n");
    printf("Client connected at IP: %s and port: %i\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

    
    close(sock);
    close(receivedSock);
}