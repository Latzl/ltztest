/*
    @brief tcli: t(est) c(ommand) l(ine) i(nterface)
 */
#ifndef TCLI_HPP
#define TCLI_HPP

#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <iomanip>

#include <ltz/proc_init/proc_init.hpp>

namespace tcli {

extern int argc_raw;
extern char** argv_raw;
extern std::vector<std::string> args_pass2fn, args_fn_path;

struct node : ltz::proc_init::fn::node {
    std::string desc;
};

class Timer {
   public:
    Timer() : tStart(std::chrono::steady_clock::now()) {}
    enum class Unit { ns, us, ms, s };

    inline Timer& end() {
        tEnd = std::chrono::steady_clock::now();
        return *this;
    }

    inline std::string report() const {
        std::stringstream ss;
        auto pr = elapsed();
        ss << std::fixed << std::setprecision(2) << pr.first << Timer::toStr(pr.second);
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
                return " ns";
            case Unit::us:
                return " us";
            case Unit::ms:
                return " ms";
            case Unit::s:
                return " s";
            default:
                assert(false);
        }
    }
};

// ltz::proc_init::Register& get_register();

void list(const std::vector<std::string>& vArgsAsFnPath);

void list_all();

void prompt(const std::vector<std::string>& vArgsAsFnPath);

namespace ipc {
int listen();
int connect();
}  // namespace ipc

int main(int argc, char* argv[]);

}  // namespace tcli

#define TCLI_GET_REG() LTZ_PI_FN_GET_REG(tcli)
#define TCLI_FN(...)                                           \
    LTZ_PI_FN_NODE_CONSTRUCT(tcli, ::tcli::node, __VA_ARGS__); \
    LTZ_PI_FN_DEF_INIT(tcli, __VA_ARGS__) {}                   \
    LTZ_PI_FN_DEF_CLEAN(tcli, __VA_ARGS__) {}                  \
    LTZ_PI_FN_DEF_MAIN(tcli, __VA_ARGS__)

#define TCLI_DF(_desc, ...)                                    \
    LTZ_PI_FN_NODE_CONSTRUCT(tcli, ::tcli::node, __VA_ARGS__); \
    LTZ_PI_FN_NODE_HANDLE(tcli, set_desc, __VA_ARGS__) {       \
        auto& node = dynamic_cast<tcli::node&>(lpif_node);     \
        node.desc = _desc;                                     \
    }                                                          \
    LTZ_PI_FN_DEF_INIT(tcli, __VA_ARGS__) {}                   \
    LTZ_PI_FN_DEF_CLEAN(tcli, __VA_ARGS__) {}                  \
    LTZ_PI_FN_DEF_MAIN(tcli, __VA_ARGS__)

#endif
