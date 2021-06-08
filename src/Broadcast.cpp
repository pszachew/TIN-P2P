#include "Broadcast.h"

#define MAX_SIZE 1024

int bytesToInt(char buffer[4]){
    int number = int((unsigned char)(buffer[0]) << 24 |
            (unsigned char)(buffer[1]) << 16 |
            (unsigned char)(buffer[2]) << 8 |
            (unsigned char)(buffer[3]));
    return number;
}

Broadcast::Broadcast(const char *name, unsigned short port){
    this->port = port;
    setIps(name);
    if ((this->sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        perror("socket() failed");
    int broadcastPermission = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
        perror("setsockopt() failed");

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(broadcastIp.c_str());
    address.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0)
        perror("bind() failed");
}
Broadcast::~Broadcast(){
    close(sock);
}
void Broadcast::broadcast(struct ResourceDetails message){
	if (sendto(sock, (void *)&message, sizeof(message), 0, (struct sockaddr *) &address, sizeof(address)) != sizeof(message))
	perror("sendto() sent a different number of bytes than expected");
}

ReceivedPacket Broadcast::receive(){
    int size;
    char buffer[MAX_SIZE];
    struct sockaddr_in received;
    socklen_t len = sizeof(received);
    if ((size = recvfrom(sock, buffer, MAX_SIZE, 0, (struct sockaddr *) &received, &len)) < 0)
        perror("recvfrom() failed");
    std::string ipReceived = inet_ntoa(received.sin_addr);
    if(ipReceived == ip){
        ResourceDetails packet;
        packet.type = SELF_SEND;
        return ReceivedPacket(packet, ipReceived);
    }
    int type = bytesToInt(buffer);
    std::string name;
    ResourceDetails packet;
    if(type == RESOURCE_LIST  || type==DELETE_RESOURCE)
    {
        packet.type = type;
        strcpy(packet.name, buffer+8);
        name = buffer + 8;
        name = name + '\0';
    } else if(type==DOWNLOAD_REQUEST)
        packet.port = bytesToInt(buffer+4);
    else perror("Wrong type of package");


    return ReceivedPacket(packet, ipReceived);
}

void Broadcast::setIps(const char *name){
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa, *sn;
    struct in_addr sb;
    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
            if(strcmp(ifa->ifa_name,name) == 0){
                sa = (struct sockaddr_in *) ifa->ifa_addr;
                ip = inet_ntoa(sa->sin_addr);
                std::cout << ip <<std::endl;
                unsigned long ip_addr = sa->sin_addr.s_addr;
                sn = (struct sockaddr_in *) ifa->ifa_netmask;
                std::string netmask(inet_ntoa(sn->sin_addr));
                unsigned long netmask_addr = sn->sin_addr.s_addr;
                sb.s_addr = ip_addr | ~netmask_addr;
                broadcastIp = inet_ntoa(sb);
            }
        }
    }

    freeifaddrs(ifap);
}

std::string Broadcast::getIp(){
    return ip;
}
