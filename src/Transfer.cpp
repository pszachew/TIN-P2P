#include "Transfer.h"

Transfer::Transfer(const char* filename, int port, std::string ip, bool sending){
    this->filename = filename;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("socket() failed");
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    if(sending)
        sendFile();
    else
        receive();
}

void Transfer::sendFile(){
    if(connect(sock, (struct sockaddr*)&address, sizeof(address)) < 0){
        perror("Unable to connect");
        exit(1);
    }
    printf("Connected with server successfully\n");
    FILE *fp;
    fp = fopen(filename, "rb");
    if(fp == NULL){
        perror("Error in reading file.");
        exit(1);
    }
    fseek(fp, 0L, SEEK_END);
    int file_length = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    int read = 0;
    struct ResourcePacketHeader packet;
    while(read != file_length){
        bzero(packet.data, CHUNK_SIZE);
        packet.size = fread(packet.data, 1, CHUNK_SIZE, fp);
        read += packet.size;
        packet.command = SEND_FILE;
        if(packet.size < CHUNK_SIZE)
            packet.command = END_OF_FILE;
        std::cout << "data: " << packet.data << std::endl;
        if(write(sock, (void*)&packet, sizeof(packet))== -1){
            perror("Error in sendung data");
            exit(1);
        }
    }
    fclose(fp);
    std::cout << "Sent\n";
    close(sock);
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

    int n;
    FILE *fp;

    fp = fopen(filename, "wb");
    if(fp==NULL){
        perror("[-]Error in creating file.");
        exit(1);
    }
    struct ResourcePacketHeader packet;
    while(true){
        n = read(receivedSock, (void*)&packet, CHUNK_SIZE);
        if(n<=0){
            fclose(fp);
            break;
        }
        char buffer[packet.size] = {0};
        strncpy(buffer, packet.data, packet.size);
        
        std::cout <<"data: " << buffer << std::endl;
        fwrite(buffer, 1, packet.size, fp);
    }

    std::cout << "Received\n";
    close(sock);
    close(receivedSock);
}
