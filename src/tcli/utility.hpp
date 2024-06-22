#ifndef SRC_TCLI_UTILITY_HPP
#define SRC_TCLI_UTILITY_HPP

#include <chrono>
#include <sstream>
#include <iomanip>

namespace tcli {

template <typename InputIt, typename UnaryOp>
inline std::string toStr(InputIt first, InputIt last, UnaryOp op) {
    std::string str;
    for (InputIt it = first; it != last; it = std::next(it)) {
        str += op(it) + " ";
    }
    if (!str.empty()) {
        str.pop_back();
    }
    return str;
}

template <typename InputIt>
inline std::string toStr(InputIt first, InputIt last) {
    return toStr(first, last, [](InputIt it) { return *it; });
}


class Timer {
   public:
    Timer() : tStart(std::chrono::steady_clock::now()) {}
    enum class Unit { ns, us, ms, s };

    inline Timer& end() {
        tEnd = std::chrono::steady_clock::now();
        return *this;
    }

    inline std::string toStr_pass_time() const {
        if (tEnd.time_since_epoch().count() == 0) {
            return "timer not stoped yet";
        }
        std::stringstream ss;
        auto pr = elapsed();
        ss << std::fixed << std::setprecision(2) << pr.first << " " << Timer::toStr(pr.second);
        return ss.str();
    }

   private:
    std::chrono::time_point<std::chrono::steady_clock> tStart, tEnd;
    inline std::pair<double, Unit> elapsed() const {
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(tEnd - tStart);
        double ns = duration.count();
        Unit unit;
        if (ns < 1e3) {
            unit = Unit::ns;
        } else if (ns < 1e6) {
            ns /= 1e3;
            unit = Unit::us;
        } else if (ns < 1e9) {
            ns /= 1e6;
            unit = Unit::ms;
        } else {
            ns /= 1e9;
            unit = Unit::s;
        }
        return {ns, unit};
    }
    static inline std::string toStr(Unit unit) {
        switch (unit) {
            case Unit::ns:
                return "ns";
            case Unit::us:
                return "us";
            case Unit::ms:
                return "ms";
            case Unit::s:
                return "s";
            default:
                throw std::domain_error("invalid unit: " + std::to_string((int)unit));
        }
    }
};

}  // namespace tcli
#endif