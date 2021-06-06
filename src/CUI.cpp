#include "CUI.h"
#include "Structures.h"

bool isNumber(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

CUI::CUI(){
    console = std::thread(&CUI::run, this);
}

void CUI::run(){
    int option = 0;
    running = true;
    while(option != 2){
        std::cout << "\x1B[2J\x1B[H";
        std::cout << "Choose:" << std::endl;
        std::cout << "1. Show resources list" << std::endl;
        std::cout << "2. Exit" << std::endl;
        std::string buffer;
        std::cin >> buffer;
        if(!isNumber(buffer)){
            std::cout << "\x1B[2J\x1B[H";
            std::cout << "You should choose number!" << std::endl;
            std::cout << "press any key to continue" << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            getchar();
            continue;
        }
        option = atoi(buffer.c_str());
        if(option == 1){ //list
            std::cout << "\x1B[2J\x1B[H";
            for(std::set<std::string>::iterator iter = resources.begin(); iter != resources.end(); ++iter)
                std::cout << * iter << ' '<<std::endl;
            std::cout << "press any key to continue" << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            getchar();
        }else if(option == 2){
            option = 2;
        }else{
            std::cout << "\x1B[2J\x1B[H";
            std::cout << "Wrong option!" << std::endl;
            std::cout << "press any key to continue" << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            getchar();
        }
    }
    running = false;
}

void CUI::updateList(std::set<std::string> list){
    resources = list;
}

bool CUI::isRunning(){
    return running;
}

void CUI::joinThread(){
    console.join();
}