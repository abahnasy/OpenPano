/*
map<std::string, std::string>
    - get(key)
    - get(key, default_value)
    - has(key)
    - remove(key)
    - clear()
    - to_string()
    - keys()
    - values()
    - items()
    - size()
    - empty()
    - operator[]
    - operator==
    - operator!=
*/
/*
TODO: change the dict style to std::unordered_map<std::string, std::string> instead of std::unordered_map<std::string, double>
use stoi, stod for numerical conversions
use define for bool conversions by comparing to "true" or "false"
*/

#ifndef OPENPANO_CONFIG_PARSER
#define OPENPANO_CONFIG_PARSER

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>

namespace Config {

    class ConfigParser{
    public:
        ConfigParser(std::filesystem::path& config_path);
        ConfigParser(const ConfigParser&) = default;
        ConfigParser(ConfigParser&&) = default;
        ConfigParser& operator=(const ConfigParser&) = default;
        ConfigParser& operator=(ConfigParser&&) = default;
        ~ConfigParser() = default;

        double get(const std::string& key) const;
        // get a value from the dictionary, if the key does not exist, throw an exception
        
        // // check the existence of a key
        bool has(const std::string& key) const;
        // // remove a key from the dictionary
        void remove(const std::string& key);
        // // clear the dictionary
        void clear();
        // // print dict
        std::string to_string() const;
        // // return keys
        std::vector<std::string> keys() const;
        // // return values, ?? not sure if it is doable or not
        std::vector<std::string> values() const;

        std::vector<std::pair<std::string, std::string>> items() const;

        size_t size() const;

        bool empty() const;
        template <typename T>
        T operator[](const std::string& key) const;

        const std::string& operator[](const std::string& key) const;

        // bool operator==(const ConfigParser& other) const;

        // bool operator!=(const ConfigParser& other) const;
    
    private:
        std::unordered_map<std::string, double> dict_;
    

    };
}

#endif