#include "Broadcast.h"
#include "Transfer.h"
#include "CUI.h"

#define BC_PORT 8080

int main(int argc, char *argv[]){
    if (argc != 2 && argc != 3){
        fprintf(stderr,"Usage: %s <send/receive> <interface>\n", argv[0]);
        exit(1);
    }
    std::string name;
    if(argc == 3)
        name = argv[2];
    else
        name = "eth0";
    Broadcast socket(BC_PORT, name.c_str()); 
    if(strcmp(argv[1], "send") == 0){
        socket.broadcast("hello", strlen("hello"), 0);
    }
    else if(strcmp(argv[1], "receive") == 0){
        std::string message = socket.receive();
        std::cout<<"Received: " << message <<std::endl; 
    }

}