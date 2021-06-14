#include "Broadcast.h"

#define MAX_SIZE 1024

Broadcast::Broadcast(const char *name, unsigned short port){
    this->port = port;
    setIps(name); // znajdz adresy ip w zaleznosci od wybranego interfejsu
    if ((this->sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) // otworzenie gniazda
        perror("socket() failed");
    int broadcastPermission = 1; // ustawienie gniazda na rozglaszanie
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
        perror("setsockopt() failed");

    // odpowiednie wartosci adresu gniazda
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(broadcastIp.c_str());
    address.sin_port = htons(port);

    // przypisanie adresu do gniazda
    if (bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0)
        perror("bind() failed");
}

Broadcast::~Broadcast(){ // zamkniecie gniazda
    close(sock);
}
void Broadcast::broadcast(struct ResourceDetails message){
    // wyslanie pakietu na adres rozgloszeniowy
	if (sendto(sock, (void *)&message, sizeof(message), 0, (struct sockaddr *) &address, sizeof(address)) != sizeof(message))
	perror("sendto() sent a different number of bytes than expected");
}

ReceivedPacket Broadcast::receive(){
    int size;
    // char buffer[MAX_SIZE];
    struct sockaddr_in received;
    socklen_t len = sizeof(received);
    ResourceDetails packet;
    // odebranie pakietu z adresu rozgloszeniowego
    if ((size = recvfrom(sock, (ResourceDetails*)&packet, sizeof(packet), 0, (struct sockaddr *) &received, &len)) < 0)
        perror("recvfrom() failed");
    std::string ipReceived = inet_ntoa(received.sin_addr); // ip klienta od ktorego otrzymalismy pakiet
    packet.type = ntohl(packet.type); // typ pakietu
    packet.port = ntohl(packet.port); // port != 0 jesli jest to zapytanie o pobranie zasobu
    std::string temp = packet.name + '\0'; // nazwa pakietu
    strcpy(packet.name, temp.c_str());
    if(ipReceived == ip){ // sprawdzenie czy nie odebralismy wlasnego pakietu
        packet.type = SELF_SEND;
        return ReceivedPacket(packet, ipReceived);
    }

    // int type = bytesToInt(buffer);
    // std::string name;
    // if(type == RESOURCE_LIST  || type==DELETE_RESOURCE)
    // {
    //     packet.type = type;
    //     strcpy(packet.name, buffer+8);
    //     name = buffer + 8;
    //     name = name + '\0';
    // } else if(type==DOWNLOAD_REQUEST){
    //     packet.type = type;
    //     packet.port = bytesToInt(buffer+4);
    //     name = buffer + 8;
    //     name = name + '\0';
    // }
    // else perror("Wrong type of package");

    return ReceivedPacket(packet, ipReceived);
}

void Broadcast::setIps(const char *name){
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa, *sn;
    struct in_addr sb;
    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) { // sprawdzenie wszystkich interfejsow po kolei 
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
            if(strcmp(ifa->ifa_name,name) == 0){ // jesli nazwa sie zgadza
                sa = (struct sockaddr_in *) ifa->ifa_addr;
                ip = inet_ntoa(sa->sin_addr);
                // std::cout << ip <<std::endl;
                unsigned long ip_addr = sa->sin_addr.s_addr; // lokalny adres ip
                sn = (struct sockaddr_in *) ifa->ifa_netmask;
                std::string netmask(inet_ntoa(sn->sin_addr)); // lokalna maska sieciowa
                unsigned long netmask_addr = sn->sin_addr.s_addr;
                sb.s_addr = ip_addr | ~netmask_addr; // otrzymanie adresu sieciowego z adresu ip i maski
                broadcastIp = inet_ntoa(sb); // adres rozgloszeniowy
            }
        }
    }
    freeifaddrs(ifap);
}

std::string Broadcast::getIp(){
    return ip;
}
