#include <iostream>
#include <filesystem>
#include "common/config_parser.h"
#include "common/config_dict.h"
#include "utils/debug.h"
#include "cstring"


void test_matcher() {
    std::cout << "Testing SIFT" << std::endl;
    
}

int main(int arc, char** argv) {

    std::cout << "Hello, World!" << std::endl;
    // build configurator object and read configuration
    // TODO: rread the configruation from config folder
     
    std::filesystem::path config_path =  std::filesystem::path {"/Users/ahmedbahnasy/workspace/myrepos/OpenPano/config.cfg"}; // TODO:fix
    // std::cout << std::filesystem::current_path() << std::endl;
    // print_path_info(config_path);
    std::cout << config_path.c_str() << std::endl;


    if (std::filesystem::exists(config_path)) {
        std::cout << "Config file exists!" << std::endl;
    } else {
        std::cout << "Config file does not exist!" << std::endl;
    }
    Config::ConfigParser config_parser(config_path);
    std::cout << config_parser.to_string() << std::endl;
    // Config::ConfigDict config_dict(config_parser);
    Config::ConfigDict config_dict (config_parser);
    std::cout << config_parser.get("CYLINDER") << std::endl;
    // do the action based on the configuration mode
    
    

    return 0;
}