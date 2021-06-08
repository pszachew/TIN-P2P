#include "CUI.h"

bool isNumber(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

CUI::CUI(std::string ip){
    this->ip = ip;
    console = std::thread(&CUI::run, this);
}

void CUI::run(){
    int option = 0;
    running = true;
    while(option != 6){
        std::cout << "\x1B[2J\x1B[H";
        std::cout << "Choose:" << std::endl;
        std::cout << "1. Show available resources list" << std::endl;
        std::cout << "2. Show local resources list" << std::endl;
        std::cout << "3. Show remote resources list" << std::endl;
        std::cout << "4. Delete resource" << std::endl;
        std::cout << "5. Download resource" << std::endl;
        std::cout << "6. Exit" << std::endl;
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
            std::cout << "\x1B[2J\x1B[H";
            for(std::set<std::string>::iterator iter = resources.begin(); iter != resources.end(); ++iter)
                std::cout << * iter << ' '<<std::endl;
            std::cout << "press any key to continue" << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            getchar();
        }
        else if(option == 2)
        {
            std::cout << "\x1B[2J\x1B[H";
            for (auto i = local_resources.begin(); i != local_resources.end(); ++i)
                std::cout << *i << std::endl;
            std::cout << "press any key to continue" << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            getchar();
        }
        else if(option == 3)
        {
            std::cout << "\x1B[2J\x1B[H";
            for (auto i = remote_resources.begin(); i != remote_resources.end(); ++i)
                std::cout << *i << std::endl;
            std::cout << "press any key to continue" << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            getchar();
        }
        else if(option == 4){
            int n = 1;
            int choose = 0;
            std::cout << "\x1B[2J\x1B[H";
            std::cout << "Choose number to delete resource:" << std::endl;   
            for (auto i = local_resources.begin(); i != local_resources.end(); ++i){
                std::cout << n <<". "<< *i << std::endl;   
                ++n;
            }
            std::cout << n <<". Go back"<< std::endl; 
            std::cin >> choose;
            if(choose == n){
                option = 0;
                continue;
            }
            n = 1;
            for (auto i = local_resources.begin(); i != local_resources.end(); ++i){
                if(choose == n){
                    deleted_resources.insert(*i);
                    local_resources.erase(*i);
                    resources.erase(*i);
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
        else if(option == 5){
            int n = 1;
            int choose = 0;
            std::cout << "\x1B[2J\x1B[H";
            std::cout << "Choose number to download resource:" << std::endl;   
            for (auto i = remote_resources.begin(); i != remote_resources.end(); ++i){
                if(std::find_if(requests.begin(), requests.end(), [&](const std::pair<std::string,int> &e){return e.first == *i;}) == requests.end() || !requests.size()){
                    std::cout << n <<". "<< *i << std::endl;
                    ++n;
                }
            }
            std::cout << n <<". Go back"<< std::endl; 
            std::cin >> choose;
            if(choose == n){
                option = 0;
                continue;
            }
            n = 1;
            for (auto i = remote_resources.begin(); i != remote_resources.end(); ++i){
                if(choose == n){
                    downloads.push_back(std::thread(&CUI::downloading, this, *i));
                    std::cout << "press any key to continue" << std::endl;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
                    getchar();
                    option = 0;
                    break;
                } else if(std::find_if(requests.begin(), requests.end(), [&](const std::pair<std::string,int> &e){return e.first == *i;}) == requests.end() || !requests.size()){
                    ++n;
                }
            }

        }
        else if(option == 6){
            option = 6;
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

void CUI::updateList(){
    updateLocal();
    resources.insert(local_resources.begin(), local_resources.end());
    for(auto const& value: remote_resources) {
        resources.insert(value);
    }
    for(auto const& value: deleted_resources) {
        resources.erase(value);
    }
}

void CUI::updateLocal(){
    std::string path = std::filesystem::current_path();
    path = path + "/resources";
    for (const auto & entry : std::filesystem::directory_iterator(path)){
        local_resources.insert(entry.path().filename());
    }
    resources.insert(local_resources.begin(), local_resources.end());
}


void CUI::downloading(std::string name){
    std::cout << "Download...\n";
    std::ofstream *f = new std::ofstream("bin/logs/received_transfers.log", std::ios::app);
    Transfer transfer(("resources/"+name), f, ip, false);
    std::pair<std::string,int> p(name, transfer.getPort());
    std::cout << "Waiting for transfer...\n";
    requests.insert(p);
    transfer.receive();
    requests.erase(p);
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
std::set<std::string> CUI::getRemote(){
    return remote_resources;
}
std::set<std::string> CUI::getAvailable(){
    return resources;
}
std::set<std::pair<std::string, int>> CUI::getRequests(){
    return requests;
}
void CUI::addDeletedResource(std::string name){
    std::string temp = "resources/";
    temp = temp + name;
    if(access(temp.c_str(), F_OK) == 0){
        if(remove(temp.c_str()) != 0)
            perror("Error deleting file");
        }
    deleted_resources.insert(name);
    remote_resources.erase(name);
    resources.erase(name);
}
void CUI::addRemoteResource(std::string name){
    if(deleted_resources.find(name) == deleted_resources.end() || !deleted_resources.size()){
        remote_resources.insert(name);
    }
}