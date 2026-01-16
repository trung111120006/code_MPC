#include "decoder.h"
#include "config.h"
#include "file_utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

namespace Decoder {
    bool checkDecodeScript() {
        return FileUtils::exists(Config::DECODE_SCRIPT_PATH);
    }
    
    bool decodeSegmentAsync(const std::string& binFilePath, 
                           int segmentNum, 
                           const std::string& qualityName) {
        std::cout << "\n┌─────────────────────────────────────────┐" << std::endl;
        std::cout << "│  Starting Decode Process                │" << std::endl;
        std::cout << "└─────────────────────────────────────────┘" << std::endl;
        
        if (!FileUtils::exists(binFilePath)) {
            std::cerr << "Error: Binary file not found: " << binFilePath << std::endl;
            return false;
        }
        
        if (!checkDecodeScript()) {
            std::cerr << "Error: Decode script not found: " 
                      << Config::DECODE_SCRIPT_PATH << std::endl;
            return false;
        }
        
        FileUtils::createDirectory(Config::DECODED_OUTPUT_DIR);
        
        std::cout << " Input:  " << binFilePath << std::endl;
        std::cout << " Running decode script..." << std::endl;
        
        std::stringstream logFilename;
        logFilename << Config::DECODED_OUTPUT_DIR << "/decode_segment_" 
                    << std::setfill('0') << std::setw(3) << segmentNum << ".log";
        std::string logFile = logFilename.str();
        
        std::stringstream cmd;
        cmd << "bash " << Config::DECODE_SCRIPT_PATH
            << " " << binFilePath
            << " " << segmentNum
            << " " << qualityName;
        
        if (Config::DECODE_BACKGROUND) {
            cmd << " > " << logFile << " 2>&1 &";
            std::cout << "   (Background mode)" << std::endl;
        } else {
            cmd << " 2>&1 | tee " << logFile;
        }
        
        std::cout << " Command: " << cmd.str() << std::endl;
        
        int result = system(cmd.str().c_str());
        
        if (Config::DECODE_BACKGROUND || result == 0) {
            std::cout << "Decode process " 
                      << (Config::DECODE_BACKGROUND ? "started" : "completed") 
                      << " successfully" << std::endl;
            return true;
        } else {
            std::cerr << " Decode failed with exit code: " << result << std::endl;
            return false;
        }
    }
}
