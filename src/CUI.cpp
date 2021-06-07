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
    while(option != 5){
        // std::cout << "\x1B[2J\x1B[H";
        std::cout << "Choose:" << std::endl;
        std::cout << "1. Show remote resources list" << std::endl;
        std::cout << "2. Show local resources list" << std::endl;
        std::cout << "3. Delete resource" << std::endl;
        std::cout << "4. Download resource" << std::endl;
        std::cout << "5. Exit" << std::endl;
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
        buffer.clear();
        if(option == 1){ //list
            // std::cout << "\x1B[2J\x1B[H";
            for(std::set<std::string>::iterator iter = remote_resources.begin(); iter != remote_resources.end(); ++iter)
                std::cout << * iter << ' '<<std::endl;
            std::cout << "press any key to continue" << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            getchar();
        }
        else if(option == 2)
        {
            // std::cout << "\x1B[2J\x1B[H";
            for (auto i = local_resources.begin(); i != local_resources.end(); ++i)
                std::cout << *i << std::endl;
            std::cout << "press any key to continue" << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            getchar();
        }
        else if(option == 3){
            int n = 1;
            int choose = 0;
            // std::cout << "\x1B[2J\x1B[H";
            std::cout << "Choose number to delete resource:" << std::endl;   
            for (auto i = local_resources.begin(); i != local_resources.end(); ++i){
                std::cout << n <<". "<< *i << std::endl;   
                ++n;
            }  
            std::cin >> choose;
            n = 1;
            for (auto i = local_resources.begin(); i != local_resources.end(); ++i){
                if(choose == n){
                    deleted_resources.insert(*i);
                    local_resources.erase(*i);
                    remote_resources.erase(*i);
                    std::string temp = "resources/";
                    temp = temp + *i;
                    if( remove(temp.c_str()) != 0 )
                        perror( "Error deleting file" );
                    else
                        puts( "File successfully deleted" );
                    std::cout << "press any key to continue" << std::endl;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                    getchar();
                    option = 0;
                    break;
                }
                ++n;
            }
        }
        else if(option == 4){

        }
        else if(option == 5){
            option = 5;
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

void CUI::updateList(std::set<std::string> avail, std::set<std::string> del){
    std::string path = std::filesystem::current_path();
    path = path + "/resources";
    for (const auto & entry : std::filesystem::directory_iterator(path)){
        local_resources.insert(entry.path().filename());
    }
    remote_resources.insert(avail.begin(), avail.end());
    deleted_resources.insert(del.begin(), del.end());
    for(auto const& value: deleted_resources) {
        remote_resources.erase(value);
    }
}

bool CUI::isRunning(){
    return running;
}

void CUI::joinThread(){
    console.join();
}
std::set<std::string> CUI::getDeleted(){
    return deleted_resources;
}
std::set<std::string> CUI::getAvailable(){
    return deleted_resources;
}