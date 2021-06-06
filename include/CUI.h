#ifndef CUI_H
#define CUI_H

#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <thread>

class CUI{
    std::vector<std::string> resources;
    std::thread console;

public:
    CUI();
    void run();

};

#endif
