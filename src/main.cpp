#include <filesystem>
#include <vector>
#include <set>
#include <pthread.h>

#include "Broadcast.h"
#include "Transfer.h"

std::vector<std::string> createList(){
    std::string path = std::filesystem::current_path();
    path = path + "/resources";
    std::vector<std::string> list;
    for (const auto & entry : std::filesystem::directory_iterator(path)){
        list.push_back(entry.path().filename());
    }
    return list;
}

void* broadcastList(void *args)
{
    Broadcast socket = *((Broadcast *)args);
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

    std::set <std::string> available_resources;
    std::set <std::string> deleted_resources;

    pthread_t broadcast_id;
    struct  ResourceDetails message;
    pthread_create(&broadcast_id, NULL, broadcastList, &socket);
    while(true){
        message = socket.receive();
        if(message.type == RESOURCE_LIST )
        {
          available_resources.insert(message.name);
        }
        else if (message.type == DELETE_RESOURCE)
        {
          available_resources.erase(message.name);
          deleted_resources.insert(message.name);
        }
        else if (message.type == DOWNLOAD_REQUEST)
        {
          
        }
        else perror("Wrong msg type");
        //std::cout<<"Received: " << message.name <<std::endl;

        for(std::set<std::string>::iterator iter = available_resources.begin(); iter != available_resources.end(); ++iter )
        std::cout << * iter << ' '<<std::endl;

        std::cout<<std::endl;
        sleep(1);
    }
}
