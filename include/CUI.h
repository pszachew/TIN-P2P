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
    std::set<std::string> resources;
    std::set<std::string> local_resources;
    std::set<std::string> remote_resources;
    std::set<std::string> deleted_resources;
    std::set<std::pair<std::string, int>> requests;
    std::vector<std::thread> downloads;


    std::thread console;
    bool running;
    std::string ip;

    void downloading(std::string name);

public:
    CUI(std::string ip);
    void run();
    bool isRunning();
    void updateList();
    void updateLocal();
    void joinThread();
    std::set<std::string> getDeleted();
    std::set<std::string> getRemote();
    std::set<std::string> getAvailable();
    std::set<std::pair<std::string, int>> getRequests();

    void addDeletedResource(std::string name);
    void addRemoteResource(std::string name);
};

#endif
