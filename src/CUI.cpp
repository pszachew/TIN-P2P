#include "CUI.h"
#include "Structures.h"

bool isNumber(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

CUI::CUI(){
    console = std::thread(&CUI::run, this);
}

void CUI::run(){
    int option = 1;
    while(option != 2){
        std::cout << "\x1B[2J\x1B[H";
        std::cout << "Choose:" << std::endl;
        std::cout << "1. Show resources list" << std::endl;
        std::cout << "2. Exit" << std::endl;
        std::string buffer;
        std::cin >> buffer;
        if(!isNumber(buffer)){
            std::cout << "You should choose number!" << std::endl;
            std::cout << "press any key to continue" << std::endl;
            std::cin.ignore();
            continue;
        }
        option = atoi(buffer.c_str());
        switch(option){
        case 1: //list
            continue;
        case 2: //exit
            break;
        default:
            std::cout << "Wrong option!" << std::endl;
            std::cout << "press any key to continue" << std::endl;
            std::cin.ignore();
        };
    }
}