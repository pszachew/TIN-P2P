#include "Broadcast.h"

#define MAXRECVSTRING 255

Broadcast::Broadcast(unsigned short port, char const* interface){
    this->port = port;
    if ((this->sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        perror("socket() failed");
    int broadcastPermission = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
        perror("setsockopt() failed");

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(findBroadcastIp(interface).c_str());
    address.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0)
        perror("bind() failed");
}
std::string Broadcast::findBroadcastIp(char const *name){
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa, *sn;
    std::string addr;
    std::string netmask;

    struct in_addr broadcast_addr;
    std::string broadcast;

    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
            if(strcmp(ifa->ifa_name, name) == 0){
                sa = (struct sockaddr_in *) ifa->ifa_addr;
                addr = inet_ntoa(sa->sin_addr);
                sn = (struct sockaddr_in *) ifa->ifa_netmask;
                netmask = inet_ntoa(sn->sin_addr);
                broadcast_addr.s_addr = sa->sin_addr.s_addr | ~(sn->sin_addr.s_addr);
                broadcast = inet_ntoa(broadcast_addr);
            }
        }
    }

    freeifaddrs(ifap);
    return broadcast;
}

void Broadcast::broadcast(char const *message, int msgSize,  int id){
    while(true){
        if (sendto(sock, message, msgSize, 0, (struct sockaddr *) &address, sizeof(address)) != msgSize)
            perror("sendto() sent a different number of bytes than expected");
        sleep(1);
    }
}

std::string Broadcast::receive(){
    int msgSize;
    char message[MAXRECVSTRING+1];
    if ((msgSize = recvfrom(sock, message, MAXRECVSTRING, 0, NULL, 0)) < 0)
        perror("recvfrom() failed");
    message[msgSize] = '\0';
    std::string str(message);

    return str;
}