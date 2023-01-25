#include <iostream>
#include <fstream>
#include <filesystem>

#include "config_parser.h"

namespace Config {
    ConfigParser::ConfigParser(std::filesystem::path& config_path) {
        // read file into stream
        std::ifstream config_file(config_path.c_str(), std::ifstream::in);
        // // parse the stream
        std::string line;
        while (std::getline(config_file, line)) {
            // skip empty lines
            if (line.empty()) {
                continue;
            }
            // skip comments
            if (line[0] == '#') {
                continue;
            }
            // parse the line
            std::istringstream iss(line);
            std::string key;
            double value;
            iss >> key >> value;
            dict_[key] = value;
        }
        // extract configuration values

    }
 
    double ConfigParser::get(const std::string& key) const {
        if (dict_.find(key) == dict_.end()) {
            throw std::invalid_argument("Key does not exist");
        }
        return dict_.at(key);
    }
    

    bool ConfigParser::has(const std::string& key) const {
        return dict_.find(key) != dict_.end();
    }

    void ConfigParser::remove(const std::string& key) {
        dict_.erase(key);
    }

    void ConfigParser::clear() {
        dict_.clear();
    }

    std::string ConfigParser::to_string() const {
        std::string result;
        for (const auto& item : dict_) {
            result += item.first + " " + std::to_string(item.second) + "\n";
        }
        return result;
    }

    std::vector<std::string> ConfigParser::keys() const {
        std::vector<std::string> result;
        for (const auto& item : dict_) {
            result.push_back(item.first);
        }
        return result;
    }

    std::vector<std::string> ConfigParser::values() const {
        std::vector<std::string> result;
        for (const auto& item : dict_) {
            result.push_back(std::to_string(item.second));
        }
        return result;
    }

    size_t ConfigParser::size() const {
        return dict_.size();
    }

    bool ConfigParser::empty() const {
        return dict_.empty();
    }

    // TODO: return the actual type of the value instead of string
    std::vector<std::pair<std::string, std::string>> ConfigParser::items() const {
        std::vector<std::pair<std::string, std::string>> result;
        for (const auto& item : dict_) {
            result.push_back(std::make_pair(item.first, std::to_string(item.second)));
        }
        return result;
    }

    template <typename T>
    T ConfigParser::operator[](const std::string& key) const {
        return dict_.find(key)->second;
    }
}