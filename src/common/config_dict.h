
#ifndef OPENPANO_CONFIG_DICT
#define OPENPANO_CONFIG_DICT

#include <filesystem>
#include <vector>
#include <string>
#include <unordered_map>

// #include "config_parser.h"


namespace Config {
    
    class ConfigParser; // forward declaration
    /*
    * this will take string values from the parser and convert them to the correct type
    * think of more clean way for this TDOO
    */
    class ConfigDict {
    public:
        // ConfigDict();
        ConfigDict(const ConfigParser&);
        // ConfigDict(const ConfigDict&) = default;
        // ConfigDict(ConfigDict&&) = default;
        // ConfigDict& operator=(const ConfigDict&) = default;
        // ConfigDict& operator=(ConfigDict&&) = default;
        // ~ConfigDict() = default;
        // list all the parameters needed here, give them public access throug dot
        bool CYLINDER = false;
        bool PLANAR = false;
        // // focal length in 35mm format. used in CYLINDER mode
        double FOCAL_LENGTH = 37; // 37
        // keypoint related parameters
        int SIFT_WORKING_SIZE = 800; // 800	# working resolution for sift
        int NUM_OCTAVE = 4; // 4
        int NUM_SCALE = 7; // 7
        double SCALE_FACTOR = 1.4142135623; // 1.4142135623
        double GAUSS_SIGMA = 1.4142135623; // 1.4142135623
        double GAUSS_WINDOW_FACTOR = 6; // 6

    };
}

#endif