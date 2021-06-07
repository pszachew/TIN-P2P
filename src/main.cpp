#include <filesystem>
#include <vector>
#include <set>
#include <thread>

#include "Broadcast.h"
#include "Transfer.h"
#include "CUI.h"

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

        for(auto const& value: resourcesList) {
            struct ResourceDetails resourcePacket;
            resourcePacket.type = htonl(RESOURCE_LIST);
            resourcePacket.size = htonl(value.size());
            strcpy(resourcePacket.name, value.c_str());
            socket->broadcast(resourcePacket);
        }
        for(auto const& value: deletedList) {
            struct ResourceDetails resourcePacket;
            resourcePacket.type = htonl(DELETE_RESOURCE);
            resourcePacket.size = htonl(value.size());
            strcpy(resourcePacket.name, value.c_str());
            socket->broadcast(resourcePacket);
        }
        usleep(10000);
    }
}

void broadcastReceive(Broadcast *socket, CUI *console){
    std::set <std::string> available = console->getAvailable();
    std::set <std::string> deleted = console->getDeleted();
    struct  ResourceDetails message;
    while(console->isRunning()){
        message = socket->receive();
        if(message.type == RESOURCE_LIST )
        {
            if(deleted.find(message.name) == deleted.end() || !deleted.size())
                available.insert(message.name);
        }
        else if (message.type == DELETE_RESOURCE)
        {
            available.erase(message.name);
            deleted.insert(message.name);
            std::string temp = "resources/";
            temp = temp + message.name;
            if(access( temp.c_str(), F_OK) == 0){
                if(remove(temp.c_str()) != 0)
                    perror("Error deleting file");
            }
        }
        else if (message.type == DOWNLOAD_REQUEST)
        {

        }
        else perror("Wrong msg type");
        console->updateList(available, deleted);
        usleep(10000);
    }
}


int main(int argc, char *argv[]){
    if (argc < 3){
        fprintf(stderr,"Usage: %s <interface> <port>\n", argv[0]);
        exit(1);
    }
    Broadcast socket(argv[1], atol(argv[2]));

    CUI console;
    std::thread broadcasting(broadcastList, &socket, &console);
    std::thread broadcastReceiving(broadcastReceive, &socket, &console);

    // Transfer transfer("resources/test1.txt", 2000, "25.64.115.66", true);

    console.joinThread();
    broadcasting.join();
    broadcastReceiving.join();
}
