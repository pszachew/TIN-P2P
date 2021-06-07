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

        for(auto const& value: resourcesList) {
            struct ResourceDetails resourcePacket;
            resourcePacket.type = htonl(RESOURCE_LIST);
            resourcePacket.size = htonl(value.size());
            strcpy(resourcePacket.name, value.c_str());
            socket->broadcast(resourcePacket);
        }
        sleep(1);
    }
}

void broadcastReceive(Broadcast *socket, CUI *console, std::set <std::string> *available, std::set <std::string> *deleted){

    struct  ResourceDetails message;
    while(console->isRunning()){
        message = socket->receive();
        if(message.type == RESOURCE_LIST )
        {
            available->insert(message.name);
        }
        else if (message.type == DELETE_RESOURCE)
        {
            available->erase(message.name);
            deleted->insert(message.name);
        }
        else if (message.type == DOWNLOAD_REQUEST)
        {

        }
        else perror("Wrong msg type");
        // std::cout<<"Received: " << message.name <<std::endl;

        // for(std::set<std::string>::iterator iter = available->begin(); iter != available->end(); ++iter )
        //     std::cout << * iter << ' '<<std::endl;
        // std::cout<<std::endl;
        console->updateList(*available);
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
    std::set <std::string> *available_resources = new std::set <std::string>();
    std::set <std::string> *deleted_resources = new std::set <std::string>();
    std::thread broadcasting(broadcastList, &socket, &console);
    std::thread broadcastReceiving(broadcastReceive, &socket, &console, available_resources, deleted_resources);

    // Transfer transfer("test.txt", 2000, "25.105.145.10");
    // transfer.receive();

    console.joinThread();
    broadcasting.join();
    broadcastReceiving.join();
}
