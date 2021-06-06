#ifndef CUI_H
#define CUI_H

#include <iostream>
#include <cstring>
#include <set>
#include <algorithm>
#include <thread>

#include<ios>   // for <streamsize>  
#include<limits> // for numeric_limits


class CUI{
    std::set<std::string> resources;
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
