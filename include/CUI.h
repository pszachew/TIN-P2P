#ifndef CUI_H
#define CUI_H

#include <iostream>
#include <cstring>
#include <set>
#include <algorithm>
#include <thread>
#include <filesystem>
#include <vector>
#include<ios>   // for <streamsize>
#include<limits> // for numeric_limits


class CUI{
    std::set<std::string> local_resources;
    std::set<std::string> remote_resources;
    std::set<std::string> deleted_resources;


    std::thread console;
    bool running;

public:
    CUI();
    void run();
    bool isRunning();
    void updateList(std::set<std::string> avail, std::set<std::string> del);
    void joinThread();
    std::set<std::string> getDeleted();
    std::set<std::string> getAvailable();

};

#endif
