#include <filesystem>
#include <vector>
#include <set>
#include <thread>
#include <fstream> 
#include <ctime>
#include <chrono>

#include "Broadcast.h"
#include "Transfer.h"
#include "CUI.h"

using namespace std::chrono_literals;

std::set<std::string> createList();
void broadcastList(Broadcast *socket, CUI *console);
void broadcastReceive(Broadcast *socket, CUI *console);
void transferFile(std::string ip, std::string name, std::ofstream *logFile, int port);
void writeLog(std::string filename, ResourceDetails packet);
void writeLog(std::string filename, ReceivedPacket packet);

int main(int argc, char *argv[]){
    if (argc < 3){
        fprintf(stderr,"Usage: %s <interface> <port>\n", argv[0]);
        exit(1);
    }
    Broadcast socket(argv[1], atol(argv[2]));

    CUI console(socket.getIp());
    std::thread broadcasting(broadcastList, &socket, &console);
    std::thread broadcastReceiving(broadcastReceive, &socket, &console);

    console.joinThread();
    broadcasting.join();
    broadcastReceiving.join();
}

std::set<std::string> createList(){
    std::string path = std::filesystem::current_path();
    path = path + "/resources";
    std::set<std::string> list;
    for (const auto & entry : std::filesystem::directory_iterator(path)){
        list.insert(entry.path().filename());
    }
    return list;
}

void broadcastList(Broadcast *socket, CUI *console){
    while(console->isRunning()){
        std::set<std::string> resourcesList = createList();
        std::set<std::string> deletedList = console->getDeleted();
        std::set<std::pair<std::string, int>> requestedList = console->getRequests();
        std::string logFile = "bin/logs/broadcasting.log";

        for(auto const& value: resourcesList) {
            ResourceDetails resourcePacket;
            resourcePacket.type = htonl(RESOURCE_LIST);
            resourcePacket.port = 0;
            strcpy(resourcePacket.name, value.c_str());
            socket->broadcast(resourcePacket);
            writeLog(logFile, resourcePacket);
        }
        for(auto const& value: deletedList) {
            ResourceDetails resourcePacket;
            resourcePacket.type = htonl(DELETE_RESOURCE);
            resourcePacket.port = 0;
            strcpy(resourcePacket.name, value.c_str());
            socket->broadcast(resourcePacket);
            writeLog(logFile, resourcePacket);
        }
        for(auto const& value: requestedList) {
            ResourceDetails resourcePacket;
            resourcePacket.type = htonl(DOWNLOAD_REQUEST);
            resourcePacket.port = htonl(value.second);
            strcpy(resourcePacket.name, value.first.c_str());
            socket->broadcast(resourcePacket);
            writeLog(logFile, resourcePacket);
        }
        sleep(1);
    }
}

void broadcastReceive(Broadcast *socket, CUI *console){
    ReceivedPacket message;
    std::vector<std::thread> transfers;
    std::vector<std::string> sending;
    std::string logFile = "bin/logs/received_broadcast.log";
    while(console->isRunning()){
        message = socket->receive();
        writeLog(logFile, message);
        if(message.packet.type == RESOURCE_LIST )
        {
            console->addRemoteResource(message.packet.name);
        }
        else if (message.packet.type == DELETE_RESOURCE)
        {
            console->addDeletedResource(message.packet.name);
        }
        else if (message.packet.type == DOWNLOAD_REQUEST)
        {
            if(std::find(sending.begin(), sending.end(), message.packet.name) == sending.end()){
                writeLog("bin/logs/sended_transfers.log", message);
                std::ofstream *f = new std::ofstream("bin/logs/sended_transfers.log", std::ios::app);
                sending.push_back(message.packet.name);
                std::cout << message.packet.name << std::endl;
                transfers.push_back(std::thread(transferFile, message.ip, message.packet.name, f, message.packet.port));
            }
        }
        else if(message.packet.type == SELF_SEND){
            console->updateLocal();
            continue;
        }
        else perror("Wrong msg type");
        console->updateList();
    }
}

void transferFile(std::string ip, std::string name, std::ofstream *logFile, int port){
    Transfer transfer(("resources/" + name), logFile, ip, true, port);
    std::cout<<"TRANSFERING.." <<std::endl;
    transfer.sendFile();
}

void writeLog(std::string filename, ResourceDetails packet){
    packet.type = ntohl(packet.type);
    packet.port = ntohl(packet.port);
    std::ofstream f(filename, std::ios::app);
    time_t now = time(0);
    tm *ltm = localtime(&now);
    f << "["<< std::to_string(ltm->tm_hour) << ":" << ltm->tm_min << ":" << ltm->tm_sec <<"]"; //timestamp
    if(packet.type == RESOURCE_LIST)
        f << " RESOURCE_LIST ";
    else if(packet.type == DELETE_RESOURCE)
        f << " DELETE_RESOURCE" ;
    else if(packet.type == DOWNLOAD_REQUEST){
        f << " DOWNLOAD_REQUEST " << packet.port << " ";
    }
    else if(packet.type == SELF_SEND)
        f << " SELF_SEND ";
    else
        f << " WRONG PACKET TYPE " << packet.type << " ";
    f << packet.name << std::endl;
    f.close();
}
void writeLog(std::string filename, ReceivedPacket p){
    std::ofstream f(filename, std::ios::app);
    time_t now = time(0);
    tm *ltm = localtime(&now);
    f << "["<< std::to_string(ltm->tm_hour) << ":" << ltm->tm_min << ":" << ltm->tm_sec <<"]"; //timestamp
    if(p.packet.type == RESOURCE_LIST)
        f << " RESOURCE_LIST ";
    else if(p.packet.type == DELETE_RESOURCE)
        f << " DELETE_RESOURCE" ;
    else if(p.packet.type == DOWNLOAD_REQUEST){
        f << " DOWNLOAD_REQUEST " << p.packet.port;
    }
    else if(p.packet.type == SELF_SEND)
        f << " SELF_SEND ";
    f << p.packet.name << " from " << p.ip << std::endl;
    f.close();
}