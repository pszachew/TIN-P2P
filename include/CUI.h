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
    std::set<std::string> resources;

    std::vector<std::string> local_resources;
    std::set<std::string> remote_resources;


    std::thread console;
    bool running;

public:
    CUI();
    void run();
    bool isRunning();
    void updateList(std::set<std::string> list);
    void joinThread();

};

#endif
