#ifndef TCLI_LOG_HPP
#define TCLI_LOG_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

namespace tcli {

template <typename It, typename UnaryOp>
inline std::string toStr(It itl, It itr, UnaryOp op) {
    std::string str;
    for (It it = itl; it != itr; it = std::next(it)) {
        str += op(it) + " ";
    }
    if (!str.empty()) {
        str.pop_back();
    }
    return str;
}

template <typename It>
inline std::string toStr(It itl, It itr) {
    return toStr(itl, itr, [](It it) { return *it; });
}

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