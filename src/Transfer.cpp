#include "Transfer.h"

int bytesToInt(char buffer[4]){
    int number = int((unsigned char)(buffer[0]) << 24 |
            (unsigned char)(buffer[1]) << 16 |
            (unsigned char)(buffer[2]) << 8 |
            (unsigned char)(buffer[3]));
    return number;
}
char* intToBytes(int n, char* buffer){
    buffer[0] = n >> 24;
    buffer[1] = n >> 16;
    buffer[2] = n >> 8;
    buffer[3] = n;
    return buffer+4;
}

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
            puts("bind() failed");
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
        puts("Unable to connect");
        *logFile << "Unable to connect" << std::endl;
        return;
    }
    *logFile << "Connected with server successfully"<< std::endl;
    FILE *fp;
    fp = fopen(filename.c_str(), "rb");
    fseek(fp, 0L, SEEK_END);
    int fileSize = ftell(fp);
    rewind(fp);
    if(fp == NULL){
        puts("Error in reading file.");
        *logFile << "Error in reading file." << std::endl;
        return;
    }
    // struct ResourcePacket packet;
    char *firstPacket = (char*)malloc(8 * sizeof(char));
    firstPacket = intToBytes(htonl(SEND_FILE), firstPacket);
    firstPacket = intToBytes(htonl(fileSize), firstPacket);
    firstPacket -= 8;
    write(sock, firstPacket, 8);
    free(firstPacket);
    char *buffer = (char*)malloc((CHUNK_SIZE) * sizeof(char));
    int n = 0;
    while(n != fileSize){
        int temp = fread(buffer, 1, CHUNK_SIZE, fp);
        int sent_size = write(sock, buffer, temp);
        if(sent_size == -1){
            puts("Error in sending data");
            *logFile << "Error in sending data" << std::endl;
            break;
        }
        n += sent_size;
        *logFile << "Sent chunk size: " << sent_size << std::endl;
    }
    free(buffer);
    fclose(fp);
    *logFile << "File transfered successfully" << std::endl;
    close(sock);
}
void Transfer::receive(){
    if(listen(sock, 1) < 0){
        puts("Error while listening");
        *logFile << "Error while listening" << std::endl;
        return;
    }
    *logFile << "Listening..." << std::endl;
    struct sockaddr_in clientAddress;
    socklen_t len = sizeof(clientAddress);
    int receivedSock = accept(sock, (struct sockaddr*) &clientAddress, &len);
    if (receivedSock < 0){
        puts("Can't accept connection");
        *logFile << "Can't accept connection" << std::endl;
        return;
    }
    *logFile << "Connected at IP: " << inet_ntoa(clientAddress.sin_addr) << " and port: " << ntohs(clientAddress.sin_port) << std::endl;

    int n;
    FILE *fp;
    fp = fopen(filename.c_str(), "wb");
    if(fp==NULL){
        puts("Error in creating file");
        *logFile << "Error in creating file" << std::endl;
        return;
    }
    // ResourcePacket packet;
    char *firstPacket = (char*)malloc(8 * sizeof(char));
    n = read(receivedSock, firstPacket, 8);
    int command = ntohl(bytesToInt(firstPacket));
    if(command != SEND_FILE){
        int fileSize = ntohl(bytesToInt(firstPacket+4));
        free(firstPacket);
        char *buffer = (char*)malloc((CHUNK_SIZE+8) * sizeof(char));
        int receivedSize = 0;
        while(receivedSize != fileSize){
            n = read(receivedSock, buffer, CHUNK_SIZE);
            if(n<=0){
                break;
            }
            receivedSize += n;
            *logFile << "Chunk size: " << n << std::endl;
            if(fwrite(buffer, 1, n, fp) < 0){
                *logFile << "Error while writing file" << std::endl;
            }
        }
        free(buffer);
    }
    fclose(fp);

    *logFile << "Transfer received" << std::endl;
    close(sock);
    close(receivedSock);
}
int Transfer::getPort(){
    return port;
}