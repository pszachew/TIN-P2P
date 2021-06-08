#include "Transfer.h"

Transfer::Transfer(std::string filename, std::ofstream *logFile, std::string ip, bool sending, int port){
    this->filename = filename;
    this->port = port;
    this->logFile = logFile;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        *logFile << "socket() failed" << std::endl;
    address.sin_family = AF_INET;
    address.sin_port = htons(port); // choose first available port
    address.sin_addr.s_addr = inet_addr(ip.c_str());

    if(!sending){
        if (bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0){
            perror("bind() failed");
            *logFile << "bind() failed" << std::endl;
            return;
        }
        socklen_t len = sizeof(address);
        if (getsockname(sock, (struct sockaddr *)&address, &len) != -1)
            this->port = ntohs(address.sin_port);
    }
}

void Transfer::sendFile(){
    if(connect(sock, (struct sockaddr*)&address, sizeof(address)) < 0){
        perror("Unable to connect");
        *logFile << "Unable to connect" << std::endl;
        return;
    }
    *logFile << "Connected with server successfully"<< std::endl;
    FILE *fp;
    fp = fopen(filename.c_str(), "rb");
    if(fp == NULL){
        perror("Error in reading file.");
        *logFile << "Error in reading file." << std::endl;
        return;
    }
    fseek(fp, 0L, SEEK_END);
    int file_length = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    int read = 0;
    struct ResourcePacket packet;
    while(read != file_length){
        bzero(packet.data, CHUNK_SIZE);
        packet.size = fread(packet.data, 1, CHUNK_SIZE, fp);
        read += packet.size;
        packet.command = SEND_FILE;
        if(packet.size < CHUNK_SIZE)
            packet.command = END_OF_FILE;
        std::cout << "data: " << packet.data << std::endl;
        if(write(sock, (void*)&packet, sizeof(packet))== -1){
            perror("Error in sending data");
            *logFile << "Error in sending data" << std::endl;
            break;
        }
    }
    fclose(fp);
    *logFile << "File transfered successfully" << std::endl;
    close(sock);
}
void Transfer::receive(){
    if(listen(sock, 1) < 0){
        perror("Error while listening");
        *logFile << "Error while listening" << std::endl;
        return;
    }
    *logFile << "Listening..." << std::endl;
    struct sockaddr_in clientAddress;
    socklen_t len = sizeof(clientAddress);
    int receivedSock = accept(sock, (struct sockaddr*) &clientAddress, &len);
    if (receivedSock < 0){
        perror("Can't accept connection");
        *logFile << "Can't accept connection" << std::endl;
        return;
    }
    *logFile << "Connected at IP: " << inet_ntoa(clientAddress.sin_addr) << " and port: " << ntohs(clientAddress.sin_port) << std::endl;

    int n;
    FILE *fp;
    fp = fopen(filename.c_str(), "wb");
    if(fp==NULL){
        perror("Error in creating file");
        *logFile << "Error in creating file" << std::endl;
        return;
    }
    ResourcePacket packet;
    while(packet.command != END_OF_FILE){
        n = read(receivedSock, (void*)&packet, CHUNK_SIZE);
        if(n<=0){
            break;
        }
        char buffer[packet.size] = {0};
        strncpy(buffer, packet.data, packet.size);
        
        std::cout <<"data: " << buffer << std::endl;
        fwrite(buffer, 1, packet.size, fp);
    }
    fclose(fp);

    *logFile << "Transfer received" << std::endl;
    close(sock);
    close(receivedSock);
}
int Transfer::getPort(){
    return port;
}