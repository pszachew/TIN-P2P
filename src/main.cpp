#include <filesystem>
#include <vector>
#include <set>
#include <thread>
#include <fstream> 
#include <ctime>

#include "Broadcast.h"
#include "Transfer.h"
#include "CUI.h"

using namespace std::chrono_literals;

std::set<std::string> createList(); // tworzenie listy lokalnych zasobow
void broadcastList(Broadcast *socket, CUI *console); // rozglaszanie pakietow
void broadcastReceive(Broadcast *socket, CUI *console); // odbieranie rozglaszanych pakietow
void transferFile(std::string ip, std::string name, std::ofstream *logFile, int port, std::vector<std::pair<std::string,int>> *sending); // przesylanie pliku do oczekujacego klienta
void writeLog(std::string filename, ResourceDetails packet); // obsluga plikow z logami
void writeLog(std::string filename, ReceivedPacket packet); // obsluga plikow  z logami

int main(int argc, char *argv[]){
    if (argc < 3){
        fprintf(stderr,"Usage: %s <interface> <port>\n", argv[0]);
        exit(1);
    }
    Broadcast socket(argv[1], atol(argv[2])); // stworzenie gniazda do rozglaszania

    CUI console(socket.getIp()); // stworzenie watku obslugujacego interfejs oraz listy zasobow
    std::thread broadcasting(broadcastList, &socket, &console); // watek odpowiedzialny za rozglaszanie pakietow
    std::thread broadcastReceiving(broadcastReceive, &socket, &console); // watek odpowiedzialny za odbieranie rozglaszanych pakietow

    console.joinThread();
    broadcasting.join();
    broadcastReceiving.join();
}

std::set<std::string> createList(){
    std::string path = std::filesystem::current_path(); // obecna sciezka
    path = path + "/resources"; // folder z zasobami
    std::set<std::string> list;
    for (const auto & entry : std::filesystem::directory_iterator(path)){
        std::string tempName = entry.path().filename();
        if(tempName.substr(tempName.size()-5, 5) != ".part")
            list.insert(tempName);
    }
    return list;
}

void broadcastList(Broadcast *socket, CUI *console){
    while(console->isRunning()){ // sprawdz czy interfejs caly czas dziala
        std::set<std::string> resourcesList = createList(); // lista lokalnych zasobow
        std::set<std::string> deletedList = console->getDeleted(); // zasoby usuniete
        std::set<std::pair<std::string, int>> requestedList = console->getRequests(); // lista zasobow wraz z portem potrzebnym do polaczenia TCP
        std::string logFile = "bin/logs/broadcasting.log"; // plik z zapisem wysylanych pakietow na adres rozgloszeniowym

        for(auto const& value: resourcesList) { // rozglaszanie pakietow z nazwami lokalnych zasobow
            ResourceDetails resourcePacket;
            resourcePacket.type = htonl(RESOURCE_LIST); 
            resourcePacket.port = 0; // wartosc niepotrzebna w tym pakiecie
            strcpy(resourcePacket.name, value.c_str());
            socket->broadcast(resourcePacket);
            writeLog(logFile, resourcePacket);
        }
        for(auto const& value: deletedList) { // rozglaszanie pakietow z nazwami usunietych zasobow
            ResourceDetails resourcePacket;
            resourcePacket.type = htonl(DELETE_RESOURCE);
            resourcePacket.port = 0;
            strcpy(resourcePacket.name, value.c_str());
            socket->broadcast(resourcePacket);
            writeLog(logFile, resourcePacket);
        }
        for(auto const& value: requestedList) { // rozglaszanie prosb o pobranie zasobow
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
    ReceivedPacket message; // odebrana wiadomosc
    std::vector<std::thread> transfers;
    std::vector<std::pair<std::string,int>> sending;
    std::string logFile = "bin/logs/received_broadcast.log";
    while(console->isRunning()){
        message = socket->receive();
        writeLog(logFile, message);
        if(message.packet.type == RESOURCE_LIST ) // pakiet z nazwa zdalnego zasobu
        {
            console->addRemoteResource(message.packet.name);
        }
        else if (message.packet.type == DELETE_RESOURCE) // pakiet z nazwa usunietego zasobu
        {
            console->addDeletedResource(message.packet.name);
        }
        else if (message.packet.type == DOWNLOAD_REQUEST) // prosba o pobranie zasobu
        {
            std::set<std::string> local = createList();
            if(std::find(local.begin(), local.end(), message.packet.name) != local.end()){ // sprawdzenie czy klient posiada dany zasob lokalnie
                // sprawdzenie czy nie nawiazalismy juz polaczenia z tym klientem
                if(std::find_if(sending.begin(), sending.end(), [&](const std::pair<std::string,int> &e)
                                    {return e.first == message.packet.name && e.second == (int)message.packet.port;}) == sending.end()){
                    writeLog("bin/logs/sended_transfers.log", message);
                    std::ofstream *f = new std::ofstream("bin/logs/sended_transfers.log", std::ios::app);
                    sending.push_back(std::make_pair(message.packet.name, message.packet.port)); // dodanie pakietu do listy wysylanych pakietow
                    // watek odpowiedzialny za przeslanie danych do proszacego klienta
                    transfers.push_back(std::thread(transferFile, message.ip, message.packet.name, f, message.packet.port, &sending));
                }
            }
        }
        else if(message.packet.type == SELF_SEND){ // wlasne pakiety 
            console->updateLocal();
            continue;
        }else if(message.packet.type == TIME_OUT)
            return;
        else perror("Wrong msg type");
        console->updateList(); // aktualizacja list zasobow w interfejsie 
    }
}

void transferFile(std::string ip, std::string name, std::ofstream *logFile, int port, std::vector<std::pair<std::string,int>> *sending){
    Transfer transfer(("resources/" + name), logFile, ip, true, port); // transfer pliku jako wysylajacy
    transfer.sendFile(); // nawiazanie polaczenia i wyslanie pliku
    sleep(1);
    sending->erase(std::remove(sending->begin(), sending->end(), std::make_pair(name, port)), sending->end());
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