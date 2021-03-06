#include "Transfer.h"

int bytesToInt(char buffer[4]){ // zamiana czterech charow na int
    int number = int((unsigned char)(buffer[0]) << 24 |
            (unsigned char)(buffer[1]) << 16 |
            (unsigned char)(buffer[2]) << 8 |
            (unsigned char)(buffer[3]));
    return number;
}
char* intToBytes(int n, char* buffer){ // zamiana int na tablice czterech char
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
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) // otwarcie gniazda
        *logFile << "socket() failed" << std::endl;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip.c_str());

    if(!sending){ // jesli klient ma odebrac plik
        // przypisujemy adres do gniazda
        if (bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0){
            puts("bind() failed");
            *logFile << "bind() failed" << std::endl;
            return;
        }
        socklen_t len = sizeof(address);
        // port byl rowny zero wiec zostal wybrany pierwszy otwarty port
        if (getsockname(sock, (struct sockaddr *)&address, &len) != -1)
            this->port = ntohs(address.sin_port);
    }
}
Transfer::~Transfer(){
    close(sock);
    close(receivedSock);
    logFile->close();
}

void Transfer::sendFile(){
    if(connect(sock, (struct sockaddr*)&address, sizeof(address)) < 0){ // proba nawiazania polaczenia
        *logFile << "Unable to connect" << std::endl;
        return;
    }
    *logFile << "Connected with server successfully"<< std::endl;
    std::cout<<"Transfering: " << filename.substr(filename.find("/")+1) << std::endl;
    FILE *fp;
    fp = fopen(filename.c_str(), "rb"); // otwarcie zasobu
    if(fp == NULL){
        puts("Error in reading file.");
        *logFile << "Error in reading file." << std::endl;
        return;
    }
    fseek(fp, 0L, SEEK_END); // przesuniecie wskaznika na koniec pliku
    int fileSize = ftell(fp); // otrzymanie rozmiaru pliku
    rewind(fp); // powrot na poczatek pliku

    // struct ResourcePacket packet;
    char *firstPacket = (char*)malloc(8 * sizeof(char)); // pierwszy pakiet z komenda oraz rozmiarem zasobu
    firstPacket = intToBytes(htonl(SEND_FILE), firstPacket);
    firstPacket = intToBytes(htonl(fileSize), firstPacket);
    firstPacket -= 8;
    if(write(sock, firstPacket, 8) <= 0){ // wyslanie pierwszego pakietu
        *logFile << "Error sending first packet: " << errno << std::endl;
        free(firstPacket);
        fclose(fp);
        close(sock);
        return;
    } 
    free(firstPacket); // wyczyszczenie bufora
    char *buffer = (char*)malloc((CHUNK_SIZE) * sizeof(char)); // bufor na kawalki pliku
    int n = 0;
    while(n != fileSize){ // wysylaj dopoki nie dotarlismy do konca pliku
        int temp = fread(buffer, 1, CHUNK_SIZE, fp); // wczytaj dane z pliku
        int sent_size = write(sock, buffer, temp); // wyslanie danych
        if(sent_size == -1){ // sprawdzenie czy udalo sie wyslac
            puts("Error in sending data");
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                *logFile << "Error sending packets: " << errno << std::endl;
                std::cout << "Transfering: " << filename.substr(filename.find("/")+1) << " timed out." << std::endl;
                *logFile << "Transfering: " << filename.substr(filename.find("/")+1) << " timed out." << std::endl;
            }else{    
                *logFile << "Error in sending data" << std::endl;
            }
            break;
        }
        n += sent_size; // dodanie liczby wyslanych bajtow do licznika
        // *logFile << "Sent chunk size: " << sent_size << std::endl;
    }
    free(buffer);
    fclose(fp);
    if(n == fileSize)
        *logFile << "File transfered successfully." << std::endl;
    else
        *logFile << "File transfer ended." << std::endl;
    std::cout << "Transfer "<< filename.substr(filename.find("/")+1) <<" ended." << std::endl;
    close(sock);
}
void Transfer::receive(){
    if(listen(sock, 1) < 0){ // nasluchuj polaczenia
        puts("Error while listening");
        *logFile << "Error while listening" << std::endl;
        return;
    }
    *logFile << "Listening..." << std::endl;
    struct sockaddr_in clientAddress;
    socklen_t len = sizeof(clientAddress);
    int receivedSock = accept(sock, (struct sockaddr*) &clientAddress, &len); // deskryptor gniazda
    if (receivedSock < 0){
        puts("Can't accept connection");
        *logFile << "Can't accept connection" << std::endl;
        return;
    }
    close(sock); // zamknij gniazdo aby nie umozliwiac kolejnych polaczen
    *logFile << "Connected at IP: " << inet_ntoa(clientAddress.sin_addr) << " and port: " << ntohs(clientAddress.sin_port) << std::endl;
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(receivedSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    int n;
    FILE *fp;
    fp = fopen((filename + ".part").c_str(), "wb");
    if(fp==NULL){
        puts("Error in creating file");
        *logFile << "Error in creating file" << std::endl;
        return;
    }
    // ResourcePacket packet;
    char *firstPacket = (char*)malloc(8 * sizeof(char)); // pierwszy pakiet z komenda i rozmiarem pliku
    n = read(receivedSock, firstPacket, 8); // odebranie pakietu
    if( n < 0){
        *logFile << "First packet: " << filename.substr(filename.find("/")+1) << " timed out." << std::endl;
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            remove((filename + ".part").c_str());
            *logFile << "Error reading packets: " << errno << std::endl;
            std::cout << "Download: " << filename.substr(filename.find("/")+1) << " timed out." << std::endl;
            *logFile << "Download: " << filename.substr(filename.find("/")+1) << " timed out." << std::endl;
            fclose(fp);
        }
        return;
    }
    // int command = ntohl(bytesToInt(firstPacket));
    int fileSize = ntohl(bytesToInt(firstPacket+4));
    free(firstPacket);
    char *buffer = (char*)malloc((CHUNK_SIZE+8) * sizeof(char));
    int receivedSize = 0;
    while(receivedSize != fileSize){ // odbieramy pakietu dopoki nie dotarlismy do konca pliku
        n = read(receivedSock, buffer, CHUNK_SIZE);
        if( n <= 0){
            if(errno == EAGAIN || errno == EWOULDBLOCK || receivedSize != fileSize){
                remove((filename + ".part").c_str());
                *logFile << "Error reading packets: " << errno << std::endl;
                std::cout << "Download: " << filename.substr(filename.find("/")+1) << " timed out." << std::endl;
                *logFile << "Download: " << filename.substr(filename.find("/")+1) << " timed out." << std::endl;
                free(buffer);
                fclose(fp);
                return;
            }
            break;
        }
        receivedSize += n;
        // *logFile << "Chunk size: " << n << std::endl;
        if(fwrite(buffer, 1, n, fp) < 0){
            *logFile << "Error while writing file" << std::endl;
        }
    }
    free(buffer);
    fclose(fp);
    rename((filename+".part").c_str(), filename.c_str());

    *logFile << "Download "<< filename.substr(filename.find("/")+1) <<" ended." << std::endl;
    std::cout << "Download "<< filename.substr(filename.find("/")+1) <<" ended." << std::endl;
}
int Transfer::getPort(){
    return port;
}