#ifndef LTZ_STRING_HPP
#define LTZ_STRING_HPP
#include <string>
#include <vector>

namespace ltz {
namespace str {

inline std::vector<std::string> split(const std::string &src, const std::string &delimiter) {
    if (src.empty()) {
        return {};
    }
    if (delimiter.empty()) {
        return {src};
    }
    std::vector<std::string> vRet{};
    size_t last = 0;
    size_t next = 0;
    while ((next = src.find(delimiter, last)) != std::string::npos) {
        vRet.emplace_back(src.substr(last, next - last));
        last = next + delimiter.size();
    }
    if (last < src.size()) {
        vRet.push_back(src.substr(last));
    } else if (last == src.size()) {
        vRet.push_back("");
    }
    return vRet;
}

template <typename InputIt, typename UnaryOp>
inline std::string join(InputIt first, InputIt last, const std::string &delimiter, UnaryOp uop) {
    std::string s;
    for (InputIt it = first; it != last; it = std::next(it)) {
        s += uop(it) + delimiter;
    }
    if (first != last && s.size()) {
        s.erase(s.size() - delimiter.size());
    }
    return s;
}

template <typename InputIt>
inline std::string join(InputIt first, InputIt last, const std::string &delimiter) {
    return join(first, last, delimiter, [](InputIt it) { return *it; });
}

}  // namespace str
}  // namespace ltz

#endif