#ifndef SRC_TCLI_LOG_HPP
#define SRC_TCLI_LOG_HPP

#include "utility.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

namespace tcli {

inline void log(const std::string& str) {
    std::string sLogPath = "/tmp/log.txt";
    std::ofstream ofs(sLogPath, std::ios::app);
    if (!ofs) {
        std::cerr << "Failed to open: " << sLogPath << std::endl;
        return;
    }

    std::stringstream ss_time;
    std::time_t tNow = std::time(nullptr);
    ss_time << std::put_time(std::localtime(&tNow), "%F %T");
    ofs << ss_time.str() << "\n" << str << std::endl;
}

}  // namespace tcli
#endif