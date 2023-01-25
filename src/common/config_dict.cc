
#include <iostream>
#include <fstream>
#include <filesystem>
#include "config_dict.h"

// add the configuration parameters manually in this structure, TODO: search for a better way

namespace Config {
        // ConfigDict::ConfigDict() {
        ConfigDict::ConfigDict(ConfigParser& parser) {
        CYLINDER = parser.get("CYLINDER");
        PLANAR = parser.get("PLANAR");
        FOCAL_LENGTH = parser.get("FOCAL_LENGTH");
        SIFT_WORKING_SIZE = parser.get("SIFT_WORKING_SIZE");
        NUM_OCTAVE = parser.get("NUM_OCTAVE");
        NUM_SCALE = parser.get("NUM_SCALE");
        SCALE_FACTOR = parser.get("SCALE_FACTOR");
        GAUSS_SIGMA = parser.get("GAUSS_SIGMA");
        GAUSS_WINDOW_FACTOR = parser.get("GAUSS_WINDOW_FACTOR");
        
    }

    
}