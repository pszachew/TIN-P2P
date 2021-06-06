#include "Broadcast.h"

#define MAX_SIZE 1024

Broadcast::Broadcast(char const* broadcastIp, unsigned short port){
    this->port = port;
    this->ip = broadcastIp;
    if ((this->sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        perror("socket() failed");
    int broadcastPermission = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
        perror("setsockopt() failed");

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(broadcastIp);
    address.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0)
        perror("bind() failed");
}

void Broadcast::broadcast(char const *message, int msgSize,  int id){
    if (sendto(sock, message, msgSize, 0, (struct sockaddr *) &address, sizeof(address)) != msgSize)
        perror("sendto() sent a different number of bytes than expected");
}
void Broadcast::broadcast(struct ResourceDetails message){
    if (sendto(sock, (void *)&message, sizeof(message), 0, (struct sockaddr *) &address, sizeof(address)) != sizeof(message))
        perror("sendto() sent a different number of bytes than expected");

}


std::string Broadcast::receive(){
    int size;
    char buffer[MAX_SIZE];
    if ((size = recvfrom(sock, buffer, MAX_SIZE, 0, NULL, 0)) < 0)
        perror("recvfrom() failed");
    int type = int((unsigned char)(buffer[0]) << 24 |
                (unsigned char)(buffer[1]) << 16 |
                (unsigned char)(buffer[2]) << 8 |
                (unsigned char)(buffer[3]));
    std::cout << type <<std::endl;
    std::string name = "received";

    return name;
}