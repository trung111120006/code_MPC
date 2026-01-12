// Đây là file quản lý của cấu hình của dự án 
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

namespace Config {
    // Directories
    extern const std::string DOWNLOAD_DIR;
    extern const std::string DECODED_OUTPUT_DIR;
    extern const std::string DECODE_SCRIPT_PATH;
    
    // Decode options
    extern const bool DECODE_REALTIME;
    extern const bool DECODE_BACKGROUND;
    
    // Streaming settings
    extern const std::string BASE_URL;
    extern const std::vector<std::string> QUALITY_SUFFIX;
    extern const std::vector<std::string> QUALITY_NAMES;
    
    // MPC parameters
    extern const int LOOKAHEAD_P;
    extern const double INITIAL_BUFFER_SIZE;
    extern const double REBUFFER_PENALTY;
    extern const int TOTAL_SEGMENTS;
    
    // Chunk sizes (kbps)
    extern const std::vector<double> CHUNK_SIZES;
    
    void init();
}

#endif