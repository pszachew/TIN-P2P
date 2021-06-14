#ifndef CUI_H
#define CUI_H

#include <unistd.h>

#include <utility>
#include <iostream>
#include <cstring>
#include <set>
#include <algorithm>
#include <thread>
#include <filesystem>
#include <vector>
#include <thread>
#include<ios>   // for <streamsize>
#include<limits> // for numeric_limits

#include "Structures.h"
#include "Transfer.h"

class CUI{
    std::set<std::string> resources; // lista wszystkich zasobow do ktorych klient ma dostep
    std::set<std::string> local_resources; // zasoby przechowywane lokalnie
    std::set<std::string> remote_resources; // dostepne zasoby nie posiadane lokalnie
    std::set<std::string> deleted_resources; // zasoby usuniete
    std::set<std::pair<std::string, int>> requests; // lista zasobow odnosnie ktorych wyslalismy prosby o pobranie
    std::vector<std::thread> downloads; // lista watkow odpowiedzialnych za polaczenie transferowe


    std::thread console; // watek glowny interfejsu
    bool running;
    std::string ip;

    void downloading(std::string name); // odbieranie pobieranego zasobu

public:
    CUI(std::string ip);
    void run(); // glowna petla interfejsu
    bool isRunning();
    void updateList(); // aktualizacja list zasobow
    void updateLocal(); // aktualizacja lokalnych zasobow
    void joinThread();
    std::set<std::string> getDeleted();
    std::set<std::string> getRemote();
    std::set<std::string> getAvailable();
    std::set<std::pair<std::string, int>> getRequests();

    void addDeletedResource(std::string name);
    void addRemoteResource(std::string name);
};

#endif
