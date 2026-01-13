#include "config.h"


namespace Config {
    const std::string DOWNLOAD_DIR = "/home/backne/Trung/code/code_mpc/MPC_libcurl/download";
    const std::string DECODED_OUTPUT_DIR = "/home/backne/Trung/code/code_mpc/MPC_libcurl/decoded_output";
    const std::string DECODE_SCRIPT_PATH = "/home/backne/Trung/code/code_mpc/MPC_libcurl/scripts/auto_decode.sh";
    
    const bool DECODE_REALTIME = true;
    const bool DECODE_BACKGROUND = true;
    
    const std::string SERVER_BASE_URL = "https://100.66.2.42:8443/";
    const std::string MPD_URL = "https://100.66.2.42:8443/server.mpd";

    const int LOOKAHEAD_P = 5;
    const double INITIAL_BUFFER_SIZE = 1.0;
    const double REBUFFER_PENALTY = 10.0;
    
    const std::vector<std::string> QUALITY_NAMES = {"low", "medium", "high"};
    
    void init() {
        // Initialization logic nếu cần
    }
}