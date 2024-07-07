#include "../common.hpp"
#include <ltz/string/string.hpp>

#define FN(...) FN_LTZ(str, __VA_ARGS__)
#define DF(desc, ...) DF_LTZ(desc, str, __VA_ARGS__)
#define GF(...) GF_LTZ(str, __VA_ARGS__)

FN(join) {
    std::vector<std::string> v{"a", "b", "c"};
    using ltz::str::join;

    std::cout << join(v.begin(), v.end(), ",") << std::endl;

    return 0;
}

GF(join, vec) {
    using ltz::str::join;
    std::vector<std::string> v;

    v = {};
    EXPECT_EQ(join(v.begin(), v.end(), ""), "");
    EXPECT_EQ(join(v.begin(), v.end(), ","), "");
    EXPECT_EQ(join(v.begin(), v.end(), "000"), "");

    v = {"1"};
    EXPECT_EQ(join(v.begin(), v.end(), ""), "1");
    EXPECT_EQ(join(v.begin(), v.end(), ","), "1");
    EXPECT_EQ(join(v.begin(), v.end(), "000"), "1");

    v = {"a", "b", "c"};
    EXPECT_EQ(join(v.begin(), v.end(), ","), "a,b,c");
    EXPECT_EQ(join(v.begin(), v.end(), ""), "abc");
    EXPECT_EQ(join(v.begin(), v.end(), "000"), "a000b000c");
}

GF(join, map) {
    using ltz::str::join;

    std::map<int, std::string> mp;

    auto trans = [](std::pair<int, std::string> pr) { return pr.second; };

    mp = {};
    EXPECT_EQ(join(mp.begin(), mp.end(), "", trans), "");
    EXPECT_EQ(join(mp.begin(), mp.end(), ",", trans), "");
    EXPECT_EQ(join(mp.begin(), mp.end(), "000", trans), "");

    mp = {
        {1, "1"}
    };
    EXPECT_EQ(join(mp.begin(), mp.end(), "", trans), "1");
    EXPECT_EQ(join(mp.begin(), mp.end(), ",", trans), "1");
    EXPECT_EQ(join(mp.begin(), mp.end(), "000", trans), "1");

    mp = {
        {1, "a"},
        {2, "b"},
        {3, "c"}
    };
    EXPECT_EQ(join(mp.begin(), mp.end(), ",", trans), "a,b,c");
    EXPECT_EQ(join(mp.begin(), mp.end(), "", trans), "abc");
    EXPECT_EQ(join(mp.begin(), mp.end(), "000", trans), "a000b000c");
}
