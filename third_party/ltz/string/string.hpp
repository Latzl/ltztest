#ifndef LTZ_STRING_HPP
#define LTZ_STRING_HPP
#include <string>
#include <vector>

namespace ltz {
namespace str {

inline std::vector<std::string> split_if(const std::string &src, const std::string &delimiter, std::function<bool(const std::string &)> pred) {
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
        std::string to_push = src.substr(last);
        if (pred(to_push)) {
            vRet.push_back(to_push);
        }
    } else if (last == src.size()) {
        std::string to_push = "";
        if (pred(to_push)) {
            vRet.push_back(to_push);
        }
    }
    return vRet;
}

inline std::vector<std::string> split(const std::string &src, const std::string &delimiter) {
    return split_if(src, delimiter, [](const std::string &) { return true; });
}

template <typename InputIt, typename TransFn>
inline std::string join(InputIt first, InputIt last, const std::string &delimiter, TransFn trans) {
    std::string s;
    for (InputIt it = first; it != last; it = std::next(it)) {
        s += trans(*it) + delimiter;
    }
    if (first != last && s.size()) {
        s.erase(s.size() - delimiter.size());
    }
    return s;
}

template <typename InputIt>
inline std::string join(InputIt first, InputIt last, const std::string &delimiter) {
    return join(first, last, delimiter, [](const typename std::iterator_traits<InputIt>::value_type &t) { return t; });
}

}  // namespace str
}  // namespace ltz

#endif