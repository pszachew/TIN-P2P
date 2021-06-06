#include <filesystem>
#include <vector>
#include <thread>

#include "Broadcast.h"
#include "Transfer.h"
#include "CUI.h"

std::vector<std::string> createList(){
    std::string path = std::filesystem::current_path();
    path = path + "/resources";
    std::vector<std::string> list;
    for (const auto & entry : std::filesystem::directory_iterator(path)){
        list.push_back(entry.path().filename());
    }
    return list;
}

void broadcastList(Broadcast socket){
    while(true){
    std::vector<std::string> resourcesList = createList();

        for(auto const& value: resourcesList) {
            struct ResourceDetails resourcePacket;
            resourcePacket.type = htonl(RESOURCE_LIST);
            resourcePacket.size = htonl(value.size());
            strcpy(resourcePacket.name, value.c_str());
            socket.broadcast(resourcePacket);
        }
        sleep(1);
    }
}


int main(int argc, char *argv[]){
    if (argc < 3){
        fprintf(stderr,"Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }
    Broadcast socket(argv[1], atol(argv[2])); 

    CUI console;
    std::thread broadcasting(broadcastList, socket);

    struct  ResourceDetails message;
    while(true){
        message = socket.receive();
        std::cout<<"Received: " << message.name <<std::endl;
        // std::cout<<"Received: " << message.name <<std::endl;

        sleep(1);
    }
}
