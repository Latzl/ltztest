#include <tcli/gtest.hpp>

namespace tcli {
namespace gtest {

std::string node::toStr_char(node::gtest_t t){
    switch(t){
        case node::type::all:
            return "a";
        case node::type::suit:
            return "s";
        case node::type::case_:
            return "c";
        default:
            throw std::domain_error("unknown type: " + std::to_string(t));
    }
}
std::string node::get_info(){
    std::string s, sInfo;
    s += "g{";

    auto append = [&sInfo](const std::string& s) {
        if (!sInfo.empty()) {
            sInfo += ", ";
        }
        sInfo += s;
    };

    append(toStr_char(t));

    s += sInfo;
    s += "}";

    return s;
}

int test_with_filter(const std::string& suit, const std::string& test) {
    ::testing::InitGoogleTest(&argc_raw, argv_raw);
    std::string filter, suit_ = suit, test_ = test;
    if (suit.empty()) {
        suit_ = "*";
    }
    if (test.empty()) {
        test_ = "*";
    }
    filter = suit_ + "." + test_;
    ::testing::GTEST_FLAG(filter) = filter.c_str();
    return RUN_ALL_TESTS();
}
}  // namespace gtest
}  // namespace tcli