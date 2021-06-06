#include <filesystem>
#include <vector>

#include "Broadcast.h"
#include "Transfer.h"

std::vector<std::string> createList(){
    std::string path = std::filesystem::current_path();
    path = path + "/bin/resources";
    std::vector<std::string> list;
    for (const auto & entry : std::filesystem::directory_iterator(path)){
        std::cout << entry.path().filename() << std::endl;
        list.push_back(entry.path().filename());
    }
    return list;
}

void broadcastList(Broadcast socket){
    std::vector<std::string> resourcesList = createList();

    for(auto const& value: resourcesList) {
        struct ResourceDetails resourcePacket;
        resourcePacket.header.type = htonl(RESOURCE_LIST);
        resourcePacket.size = htonl(value.size());
        strcpy(resourcePacket.name, value.c_str());
        socket.broadcast(resourcePacket);
    }


}


int main(int argc, char *argv[]){
    if (argc < 3){
        fprintf(stderr,"Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }
    Broadcast socket(argv[1], atol(argv[2])); 


    while(true){
        broadcastList(socket);
        std::string message = socket.receive();
        std::cout<<"Received: " << message <<std::endl;

        usleep(10000); // sleep for 10 ms
    }
}