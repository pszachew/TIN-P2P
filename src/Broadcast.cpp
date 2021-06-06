#include "Broadcast.h"

#define MAX_SIZE 1024

int bytesToInt(char buffer[4]){
    int number = int((unsigned char)(buffer[0]) << 24 |
            (unsigned char)(buffer[1]) << 16 |
            (unsigned char)(buffer[2]) << 8 |
            (unsigned char)(buffer[3]));
    return number;
}

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

struct ResourceDetails Broadcast::receive(){
    int size;
    char buffer[MAX_SIZE];
    if ((size = recvfrom(sock, buffer, MAX_SIZE, 0, NULL, 0)) < 0)
        perror("recvfrom() failed");
    int type = bytesToInt(buffer);
    std::string name;
    struct ResourceDetails packet;
    if(type == RESOURCE_LIST || type==DOWNLOAD_REQUEST || type==DELETE_RESOURCE)
    {
    packet.type = type;
    packet.size = bytesToInt(buffer + 4);
    strcpy(packet.name, buffer+8);
    name = buffer + 8;
    name = name + '\0';
    }
    else perror("Wrong type of package");


    return packet;
}
